#pragma once

struct __PEB
{
	BYTE Reserved1[0x10];
	PVOID ImageBaseAddress;
};

// This backend is x64-only; fail at compile time if a 32-bit build is selected.
static_assert(sizeof(void*) == 8, "The PEB reader requires a 64-bit build");
static_assert(offsetof(__PEB, ImageBaseAddress) == 0x10, "Unexpected PEB layout");

namespace mm
{
	class c_usermode
	{
	public:
		static DWORD get_process_pid(const std::string& process_name)
		{
			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (snapshot == INVALID_HANDLE_VALUE)
				return 0;

			PROCESSENTRY32 pe{};
			pe.dwSize = sizeof(pe);

			if (!Process32First(snapshot, &pe))
			{
				CloseHandle(snapshot);
				return 0;
			}

			do
			{
				if (_stricmp(pe.szExeFile, process_name.c_str()) == 0)
				{
					CloseHandle(snapshot);
					return pe.th32ProcessID;
				}
			} while (Process32Next(snapshot, &pe));

			CloseHandle(snapshot);
			return 0;
		}

		static HANDLE get_process_handle(DWORD pid)
		{
			constexpr DWORD access =
                PROCESS_QUERY_LIMITED_INFORMATION |
                PROCESS_VM_READ |
                PROCESS_VM_WRITE |
                PROCESS_VM_OPERATION;
            return OpenProcess(access, FALSE, pid);
		}

        // Diagnostic information is captured at each failing API, rather than
        // querying GetLastError after multiple unrelated operations.
        struct s_base_diagnostic
        {
            const char* stage = "not_started";
            bool queried = false;
            bool read_attempted = false;
            NTSTATUS query_status = 0;
            ULONG query_return_length = 0;
            DWORD win32_error = ERROR_SUCCESS;
            SIZE_T bytes_read = 0;
            uintptr_t peb_address = 0;
        };

        static uintptr_t get_base_address(HANDLE process, s_base_diagnostic* report = nullptr)
        {
            s_base_diagnostic diagnostic{};
            auto finish = [&](uintptr_t result) -> uintptr_t
            {
                if (report)
                    *report = diagnostic;
                return result;
            };

            if (!process || process == INVALID_HANDLE_VALUE)
            {
                diagnostic.stage = "invalid_handle";
                diagnostic.win32_error = ERROR_INVALID_HANDLE;
                return finish(0);
            }

            if (!GetModuleHandleA(encrypt("ntdll.dll")))
            {
                diagnostic.stage = "GetModuleHandleA";
                diagnostic.win32_error = GetLastError();
                return finish(0);
            }

            PROCESS_BASIC_INFORMATION pbi{};
            diagnostic.queried = true;
            diagnostic.query_status = NtQueryInformationProcess(
                process, ProcessBasicInformation, &pbi, sizeof(pbi),
                &diagnostic.query_return_length);
            // NTSTATUS is authoritative here; GetLastError is NOT.
            if (diagnostic.query_status != 0)
            {
                diagnostic.stage = "NtQueryInformationProcess";
                return finish(0);
            }

            diagnostic.peb_address = reinterpret_cast<uintptr_t>(pbi.PebBaseAddress);
            if (!pbi.PebBaseAddress)
            {
                diagnostic.stage = "null_peb";
                return finish(0);
            }

            __PEB peb{};
            diagnostic.read_attempted = true;
            SetLastError(ERROR_SUCCESS);
            const BOOL read_ok = ReadProcessMemory(
                process, pbi.PebBaseAddress, &peb, sizeof(peb),
                &diagnostic.bytes_read);
            if (!read_ok)
            {
                diagnostic.win32_error = GetLastError();
                diagnostic.stage = "ReadProcessMemory";
                return finish(0);
            }

            if (diagnostic.bytes_read != sizeof(peb))
            {
                diagnostic.stage = "short_peb_read";
                return finish(0);
            }

            if (!peb.ImageBaseAddress)
            {
                diagnostic.stage = "null_image_base";
                return finish(0);
            }

            diagnostic.stage = "success";
            return finish(reinterpret_cast<uintptr_t>(peb.ImageBaseAddress));
        }
	};
}
