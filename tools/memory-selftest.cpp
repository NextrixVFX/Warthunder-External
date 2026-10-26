// Independent Windows x64 diagnostic: tests ReadProcessMemory only on a child
// process created by this executable. Never opens a game or third-party process.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winternl.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

struct PebPrefix
{
    BYTE reserved[0x10];
    PVOID image_base_address;
};
static_assert(sizeof(void*) == 8, "x64 only");
static_assert(sizeof(PebPrefix) == 24, "Unexpected PEB prefix size");
static_assert(offsetof(PebPrefix, image_base_address) == 0x10, "Unexpected PEB offset");

using QueryProcess = NTSTATUS (NTAPI *)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

struct ChildGuard
{
    PROCESS_INFORMATION process{};
    ~ChildGuard()
    {
        if (process.hProcess)
        {
            // Test-only child is deliberately created suspended, so terminate it.
            TerminateProcess(process.hProcess, 0);
            WaitForSingleObject(process.hProcess, 3000);
        }
        if (process.hThread) CloseHandle(process.hThread);
        if (process.hProcess) CloseHandle(process.hProcess);
    }
};

struct HandleGuard
{
    HANDLE value = nullptr;
    ~HandleGuard() { if (value) CloseHandle(value); }
};

static bool probe(DWORD pid, const char* label, DWORD access, QueryProcess query)
{
    std::printf("[selftest] %s: access=0x%08lX, pid=%lu\n",
        label, static_cast<unsigned long>(access), static_cast<unsigned long>(pid));

    SetLastError(ERROR_SUCCESS);
    HandleGuard handle{OpenProcess(access, FALSE, pid)};
    const DWORD open_error = handle.value ? ERROR_SUCCESS : GetLastError();
    std::printf("[selftest] %s OpenProcess: success=%d Win32=%lu\n",
        label, handle.value != nullptr, static_cast<unsigned long>(open_error));
    if (!handle.value) return false;

    PROCESS_BASIC_INFORMATION pbi{};
    ULONG return_length = 0;
    const NTSTATUS ntstatus = query(
        handle.value, ProcessBasicInformation, &pbi,
        static_cast<ULONG>(sizeof(pbi)), &return_length);
    std::printf("[selftest] %s NtQueryInformationProcess: NTSTATUS=0x%08lX return_length=%lu\n",
        label, static_cast<unsigned long>(static_cast<std::uint32_t>(ntstatus)),
        static_cast<unsigned long>(return_length));
    if (ntstatus < 0 || !pbi.PebBaseAddress) return false;

    PebPrefix peb{};
    SIZE_T bytes_read = 0;
    SetLastError(ERROR_SUCCESS);
    const BOOL ok = ReadProcessMemory(
        handle.value, pbi.PebBaseAddress, &peb, sizeof(peb), &bytes_read);
    const DWORD read_error = ok ? ERROR_SUCCESS : GetLastError();
    std::printf("[selftest] %s ReadProcessMemory(PEB): success=%d bytes=%zu/%zu Win32=%lu\n",
        label, ok != FALSE, static_cast<size_t>(bytes_read), sizeof(peb),
        static_cast<unsigned long>(read_error));
    if (!ok || bytes_read != sizeof(peb) || !peb.image_base_address) return false;

    BYTE mz[2]{};
    bytes_read = 0;
    SetLastError(ERROR_SUCCESS);
    const BOOL mz_ok = ReadProcessMemory(
        handle.value, peb.image_base_address, mz, sizeof(mz), &bytes_read);
    const DWORD mz_error = mz_ok ? ERROR_SUCCESS : GetLastError();
    const bool signature_valid = mz_ok && bytes_read == sizeof(mz)
        && mz[0] == static_cast<BYTE>('M') && mz[1] == static_cast<BYTE>('Z');
    std::printf("[selftest] %s ReadProcessMemory(MZ): success=%d bytes=%zu/%zu Win32=%lu signature=%s\n",
        label, mz_ok != FALSE, static_cast<size_t>(bytes_read), sizeof(mz),
        static_cast<unsigned long>(mz_error), signature_valid ? "MZ" : "INVALID");
    if (!signature_valid) return false;

    // Negative control: intentionally invalid address in our *own* test child.
    // The API must fail; the returned Win32 code is captured at the call site.
    BYTE negative{};
    bytes_read = 0;
    SetLastError(ERROR_SUCCESS);
    const BOOL bad_ok = ReadProcessMemory(
        handle.value, reinterpret_cast<LPCVOID>(static_cast<uintptr_t>(1)),
        &negative, sizeof(negative), &bytes_read);
    const DWORD bad_error = bad_ok ? ERROR_SUCCESS : GetLastError();
    std::printf("[selftest] %s NegativeControl: success=%d bytes=%zu Win32=%lu expected_failure=1\n",
        label, bad_ok != FALSE, static_cast<size_t>(bytes_read),
        static_cast<unsigned long>(bad_error));
    return bad_ok == FALSE;
}

int main()
{
    wchar_t exe_path[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
    if (length == 0 || length == MAX_PATH)
    {
        std::printf("[selftest] GetModuleFileNameW failed: Win32=%lu\n",
            static_cast<unsigned long>(GetLastError()));
        return 2;
    }

    // Suspended means the child remains a controlled process for the full test.
    std::wstring command_line = L"\"" + std::wstring(exe_path) + L"\" --child";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    ChildGuard child{};
    SetLastError(ERROR_SUCCESS);
    if (!CreateProcessW(exe_path, &command_line[0], nullptr, nullptr, FALSE,
            CREATE_SUSPENDED | CREATE_NO_WINDOW, nullptr, nullptr,
            &startup, &child.process))
    {
        std::printf("[selftest] CreateProcessW failed: Win32=%lu\n",
            static_cast<unsigned long>(GetLastError()));
        return 2;
    }
    std::printf("[selftest] Controlled child created: pid=%lu\n",
        static_cast<unsigned long>(child.process.dwProcessId));

    const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    QueryProcess query = nullptr;
    if (ntdll)
        query = reinterpret_cast<QueryProcess>(GetProcAddress(ntdll, "NtQueryInformationProcess"));
    if (!query)
    {
        std::printf("[selftest] NtQueryInformationProcess unavailable; Win32=%lu\n",
            static_cast<unsigned long>(GetLastError()));
        return 2;
    }

    constexpr DWORD read_only = PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ;
    constexpr DWORD project_flags = read_only | PROCESS_VM_WRITE | PROCESS_VM_OPERATION;
    const bool basic_ok = probe(child.process.dwProcessId, "read_only", read_only, query);
    const bool flags_ok = probe(child.process.dwProcessId, "project_flags", project_flags, query);
    std::printf("[selftest] SUMMARY read_only=%s project_flags=%s\n",
        basic_ok ? "PASS" : "FAIL", flags_ok ? "PASS" : "FAIL");
    return (basic_ok && flags_ok) ? 0 : 1;
}
