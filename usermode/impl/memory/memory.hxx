#pragma once

#define is_invalid(value) if (!value) return false

namespace External::Runtime
{
	struct s_application
	{
		bool m_is_valid = false;
		DWORD m_pid = NULL;
		HANDLE m_handle = NULL;
		uintptr_t m_base_address = 0x0;
	};

	class c_memory
	{
	private:
		static DWORD get_pid(const std::string p_proc_name)
		{
			return mm::c_usermode::get_process_pid(p_proc_name);
		}

#ifdef is_usermode_mem
		static HANDLE get_handle(const DWORD p_pid)
		{
			return mm::c_usermode::get_process_handle(p_pid);
		}

		static uintptr_t get_base_addr(
            const HANDLE p_proc_handle,
            mm::c_usermode::s_base_diagnostic& diagnostic)
        {
            return mm::c_usermode::get_base_address(p_proc_handle, &diagnostic);
        }
#endif

	public:
		s_application m_application;

        bool initialize(const std::string& p_name)
        {
            if (m_application.m_is_valid)
                return true;

            while (m_application.m_pid == NULL)
            {
                m_application.m_pid = get_pid(p_name);
                Sleep(100);
            }
            Logger::print(encrypt("[memory] Process found. PID: %lu"), m_application.m_pid);

#ifdef is_usermode_mem
            SetLastError(ERROR_SUCCESS);
            m_application.m_handle = get_handle(m_application.m_pid);
            if (!m_application.m_handle || m_application.m_handle == INVALID_HANDLE_VALUE)
            {
                const DWORD error = GetLastError();
                Logger::print(encrypt("[diag] OpenProcess: FAILED; Win32=%lu"), error);
                return false;
            }
            Logger::print(encrypt("[diag] OpenProcess: OK"));

            mm::c_usermode::s_base_diagnostic diagnostic{};
            m_application.m_base_address = get_base_addr(m_application.m_handle, diagnostic);

            if (diagnostic.queried)
                Logger::print(
                    encrypt("[diag] NtQueryInformationProcess: NTSTATUS=0x%08lX, return_length=%lu"),
                    static_cast<unsigned long>(static_cast<uint32_t>(diagnostic.query_status)),
                    static_cast<unsigned long>(diagnostic.query_return_length));

            if (diagnostic.read_attempted)
                Logger::print(
                    encrypt("[diag] ReadProcessMemory(PEB): bytes=%llu/%llu, Win32=%lu"),
                    static_cast<unsigned long long>(diagnostic.bytes_read),
                    static_cast<unsigned long long>(sizeof(__PEB)),
                    diagnostic.win32_error);

            Logger::print(encrypt("[diag] Base address stage: %s"), diagnostic.stage);

            if (!m_application.m_base_address)
            {
                Logger::print(
                    encrypt("[memory] ERROR: base address failure; stage=%s, NTSTATUS=0x%08lX, Win32=%lu, PEB=0x%llX"),
                    diagnostic.stage,
                    static_cast<unsigned long>(static_cast<uint32_t>(diagnostic.query_status)),
                    diagnostic.win32_error,
                    static_cast<unsigned long long>(diagnostic.peb_address));
                return false;
            }
#endif
            Logger::print(encrypt("Attached to:\t%s"), p_name.c_str());
            Logger::print(encrypt("Found pid:\t%lu"), m_application.m_pid);
            Logger::print(encrypt("Found base address:\t0x%llx\n"),
                static_cast<unsigned long long>(m_application.m_base_address));
            m_application.m_is_valid = true;
            return true;
        }

		bool is_valid_address(uintptr_t addr) const noexcept
		{
			return (addr > 0x10000 && addr < 0x7FFFFFFFFFFFFFFF && m_application.m_is_valid);
		}

		template<typename T>
		[[nodiscard]]
		T read(uintptr_t addr) const
		{
			if (!m_application.m_is_valid)
				return T{};

#ifdef is_usermode_mem
			T buffer{};
			if (ReadProcessMemory(m_application.m_handle, reinterpret_cast<LPCVOID>(addr), &buffer, sizeof(T), NULL))
				return buffer;	
#endif
			return T{};
		}

		bool read(uintptr_t addr, void* buffer, size_t size) const
		{
			if (!m_application.m_is_valid)
				return false;

#ifdef is_usermode_mem
			if (ReadProcessMemory(m_application.m_handle, reinterpret_cast<LPCVOID>(addr), buffer, size, NULL))
				return true;	
#endif
			return false;
		}

		template<typename T>
		bool write(uintptr_t addr, T value) const
		{
			if (!m_application.m_is_valid)
				return false;

			if (!is_valid_address(addr))
				return false;

#ifdef is_usermode_mem
			if (WriteProcessMemory(m_application.m_handle, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), NULL))
				return true;	
#endif
			return false;
		}
	};
}

External::Runtime::c_memory g_memory{};

#undef is_invalid
