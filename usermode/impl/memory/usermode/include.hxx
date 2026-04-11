#pragma once

struct __PEB
{
	BYTE Reserved1[0x10];
	PVOID ImageBaseAddress;
};

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
			return OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
		}

		static uintptr_t get_base_address(HANDLE process)
		{
			auto ntdll = GetModuleHandleA(encrypt("ntdll.dll"));
			if (!ntdll)
				return 0;
			
			PROCESS_BASIC_INFORMATION pbi{};
			if (NtQueryInformationProcess(process, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr) != 0)
				return 0;

			__PEB peb{};
			if (!ReadProcessMemory(process, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr))
				return 0;

			return reinterpret_cast<uintptr_t>(peb.ImageBaseAddress);
		}
	};
}
