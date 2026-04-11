#pragma once

unsigned long long WNF_SHEL_DESKTOP_APPLICATION_STARTED = 0x0d83063ea3be0875;
HANDLE event_handle = NULL;
enum _COMM_REQUESTS : ULONG
{
    m_is_running,
    m_unload_driver,
    m_mouse_event,
    m_get_proccess_base_addr,
    m_read_virtual,
    m_write_virtual,
    m_read_array_indexes,
    m_determine_bones_bulk,
    m_determine_entity,
    m_determine_entity_bulk,
    m_locate_actor,
    m_determine_actor
};

typedef struct _KERNEL_RUNNING_REQUEST {
    wchar_t message[128];
} KERNEL_RUNNING_REQUEST, * PKERNEL_RUNNING_REQUEST;

typedef struct _COMM_HEADER {
    ULONG Magic;
    ULONG Type;
    PVOID Instruction;
} COMM_HEADER, * PCOMM_HEADER;

typedef struct _PERIPHERAL_REQUEST
{
    long x;
    long y;
    unsigned short k;
} PERIPHERAL_REQUEST, * PPERIPHERAL_REQUEST;

typedef struct _KERNEL_BASE_REQUEST {
    UINT32 ProcessId;
    UINT64 ProcessBase;
    UINT64 DTB;
} KERNEL_BASE_REQUEST, * PKERNEL_BASE_REQUEST;

typedef struct _KERNEL_COPY_REQUEST {
    UINT32 pid;
    uintptr_t address;
    PVOID buffer;
    uintptr_t size;
    bool m_do_translate;
} KERNEL_COPY_REQUEST, * PKERNEL_COPY_REQUEST;

typedef struct _KERNEL_READ_ARRAY_INDEXES {
    uintptr_t m_address{};
    std::uint32_t indexes[128];
    std::uint32_t index_count = 0;
    std::size_t m_size{};
    PVOID m_buffer;
} KERNEL_READ_ARRAY_INDEXES, * PKERNEL_READ_ARRAY_INDEXES;

struct bone_idx
{
    std::uint32_t indexes[20];
    std::uint32_t index_count = 0;
};

struct determine_bones_in
{
    uintptr_t m_player_state = 0;
    uintptr_t m_pawn = 0;
    uintptr_t m_mesh = 0;
    uintptr_t m_component_to_world = 0;
    uintptr_t m_bone_array = 0;
};

typedef struct _KERNEL_DETERMINE_BONES_BULK
{
    uintptr_t* m_player_array;

    bone_idx bone_indexes;
    determine_bones_in m_offsets;

    // OUTPUT
    PVOID m_out;                 // per-player metadata
    PVOID m_transform_buffer; // all bones (flat)

    size_t Count;     // OUT: valid players processed
    size_t MaxCount;  // IN: max players
} KERNEL_DETERMINE_BONES_BULK, * PKERNEL_DETERMINE_BONES_BULK;


typedef struct _KERNEL_DETERMINE_ENTITY {
    entity_determine_in m_offsets;
    PVOID m_buffer;
} KERNEL_DETERMINE_ENTITY, * PKERNEL_DETERMINE_ENTITY;

typedef struct _KERNEL_DETERMINE_ENTITY_BULK {
    uintptr_t* m_player_array;
    entity_determine_in m_offsets;
    PVOID m_buffer;
    size_t Count;
    size_t MaxCount;
} KERNEL_DETERMINE_ENTITY_BULK, * PKERNEL_DETERMINE_ENTITY_BULK;

typedef struct _KERNEL_LOCATE_ACTOR {
    uintptr_t Actor = 0;
    uintptr_t RootComponent = 0;
    uintptr_t RelativeLocation = 0;
    PVOID m_buffer;
} KERNEL_LOCATE_ACTOR, * PKERNEL_LOCATE_ACTOR;

typedef struct _KERNEL_DETERMINE_ACTOR {
    uintptr_t AActor = 0;
    uintptr_t SimulatingTooLongLength = 0;
    uintptr_t PickupSpawnSource = 0;
    uintptr_t SearchPtr = 0;
    uintptr_t PickupEntry = 0;
    uintptr_t ItemRarity = 0;
    uintptr_t ItemNamePtr = 0;
    uintptr_t RootComponent = 0;
    uintptr_t RelativeLocation = 0;
    PVOID m_buffer;
} KERNEL_DETERMINE_ACTOR, * PKERNEL_DETERMINE_ACTOR;

extern "C"
{
    long __stdcall NtUpdateWnfStateData(
        void* StateName,
        void* Buffer,
        unsigned long Length,
        void* TypeId,
        void* ExplicitScope,
        unsigned long MatchingChangeStamp,
        unsigned long CheckStamp);
}

std::mutex wnf_call_lock;
SRWLOCK wnf_srwlock = SRWLOCK_INIT;

enum class ThreadPriority {
    High = 1, // ESP
    Medium = 2,
    Low = 3
};

thread_local std::stack<ThreadPriority> priority_stack;

void PushPriority(ThreadPriority priority) {
    priority_stack.push(priority);
}

void PopPriority() {
    if (!priority_stack.empty()) {
        priority_stack.pop();
    }
}

ThreadPriority GetCurrentPriority() {
    return priority_stack.empty() ? ThreadPriority::High : priority_stack.top();
}

struct ScopedPriority {
    ScopedPriority(ThreadPriority priority) { PushPriority(priority); }
    ~ScopedPriority() { PopPriority(); }
};

struct ThrottleState {
    std::mutex mtx;
    std::condition_variable cv;
    int budget_remaining = 1000; // adjustable
} throttle_state;

class c_interface {
public:
    uintptr_t processbase;
    HANDLE g_hWnfEvent;
    void CallBuffer(uint32_t type, void* instruction) {
        //std::lock_guard<std::mutex> lock(wnf_call_lock);
        AcquireSRWLockExclusive(&wnf_srwlock);

        COMM_HEADER request{};
        request.Magic = 0xAE415;
        request.Type = type;
        request.Instruction = instruction;

        void* pointer = &request;

        //std::cout << "[*] Address of request buffer: " << pointer << std::endl;

        // Also print the instruction pointer inside the struct
        //std::cout << "[*] Address of instruction payload: " << instruction << std::endl;
        NtUpdateWnfStateData(&WNF_SHEL_DESKTOP_APPLICATION_STARTED, &pointer, sizeof(uint64_t), nullptr, nullptr, 0, FALSE);
        if (WaitForSingleObject(event_handle, 0) == WAIT_FAILED) {
            // printf("Failed to lock and wait for event2\n");
        }
        ResetEvent(event_handle);
        ReleaseSRWLockExclusive(&wnf_srwlock);
    }

    DWORD GetProcessIdByName(const std::wstring& processName) {
        DWORD pid = 0;

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return 0;

        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                    pid = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return pid;
    }

    bool IsRunning() 
    {
        KERNEL_RUNNING_REQUEST request{};
        CallBuffer(_COMM_REQUESTS::m_is_running, &request);
        return (_wcsicmp(request.message, L"1") == 0);
    }

    void UnloadDriver()
    {
        CallBuffer(_COMM_REQUESTS::m_unload_driver, 0);
    }

    uint64_t GetProcessBaseAddress(ULONG PID) {
        KERNEL_BASE_REQUEST request{};
        ProcessId = PID;
        request.ProcessId = ProcessId;
        request.ProcessBase = 0;

        CallBuffer(_COMM_REQUESTS::m_get_proccess_base_addr, &request);
        //std::cout << "PID: " << request.ProcessId << std::endl;
        //std::cout << "DTB: " << request.DTB << std::endl;
        //std::cout << "Base: 0x" << std::hex << request.ProcessBase << std::endl;
        return request.ProcessBase;
    }

    template<typename T>
    T Read(uint64_t address, bool translate = true) {
        static constexpr ThreadPriority DEFAULT_PRIORITY = ThreadPriority::High;

        T result{};
        if (address == 0)
            return result;

        // Throttling
        ThreadPriority current_priority = GetCurrentPriority();
        if (current_priority != ThreadPriority::High) {
            std::unique_lock<std::mutex> lock(throttle_state.mtx);
            throttle_state.cv.wait(lock, [&] {
                return throttle_state.budget_remaining > 0;
                });
            --throttle_state.budget_remaining;
            lock.unlock();
        }

        // Actual memory read
        KERNEL_COPY_REQUEST request{};
        request.m_do_translate = translate;
        request.pid = ProcessId;
        request.size = sizeof(T);
        request.address = (unsigned long long)address;
        request.buffer = &result;

        CallBuffer(_COMM_REQUESTS::m_read_virtual, &request);

        return result;
    }

    bool Read(uintptr_t address, void* buffer, size_t size, bool translate = true) {
        if (!buffer || size == 0)
            return false;

        KERNEL_COPY_REQUEST request{};
        request.m_do_translate = translate;
        request.pid = ProcessId;
        request.address = address;
        request.buffer = buffer;
        request.size = size;

        CallBuffer(_COMM_REQUESTS::m_read_virtual, &request);
        return true; // assuming CallBuffer returns NTSTATUS
    }

    template <typename T>
    bool Write(uint64_t address, T buffer) {
        static constexpr ThreadPriority DEFAULT_PRIORITY = ThreadPriority::High;

        if (address == 0)
            return false;

        // Throttling
        ThreadPriority current_priority = GetCurrentPriority();
        if (current_priority != ThreadPriority::High) {
            std::unique_lock<std::mutex> lock(throttle_state.mtx);
            throttle_state.cv.wait(lock, [&] {
                return throttle_state.budget_remaining > 0;
                });
            --throttle_state.budget_remaining;
            lock.unlock();
        }

        // Actual memory read
        KERNEL_COPY_REQUEST request{};
        request.m_do_translate = true;
        request.pid = ProcessId;
        request.size = sizeof(T);
        request.address = (unsigned long long)address;
        request.buffer = &buffer;

        CallBuffer(_COMM_REQUESTS::m_write_virtual, &request);
        return true;
    }

    bool is_valid(const uint64_t addr)
    {
        return !(addr <= 0x400000 || addr == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(addr) == nullptr || addr > 0x7FFFFFFFFFFFFFFF);
    }

private:
    ULONG ProcessId;
} Interface;