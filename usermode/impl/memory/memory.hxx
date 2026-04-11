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

		static uintptr_t get_base_addr(const HANDLE p_proc_handle)
		{
			return mm::c_usermode::get_base_address(p_proc_handle);
		}
#else
		static uintptr_t get_base_addr(const DWORD p_pid)
		{
			return Interface.GetProcessBaseAddress(p_pid);
		}
#endif

	public:
		s_application m_application;

		bool initialize(const std::string& p_name)
		{
			if (m_application.m_is_valid)
				return true;

			// get pid
			while (m_application.m_pid == NULL)
			{
				m_application.m_pid = get_pid(p_name);
				Sleep(100);
			}

#ifdef is_usermode_mem
			// get handle
			m_application.m_handle = get_handle(m_application.m_pid);
			is_invalid(m_application.m_handle);

			// get base
			m_application.m_base_address = get_base_addr(m_application.m_handle);
			is_invalid(m_application.m_base_address);
#else
			// get base
			m_application.m_base_address = get_base_addr(m_application.m_pid);
			is_invalid(m_application.m_base_address);
#endif

			Logger::print(encrypt("Attached to:\t%s"), p_name.c_str());
			Logger::print(encrypt("Found pid:\t%d"), m_application.m_pid);
			// Logger::print(encrypt("Found handle:\t 0x%llx"), m_application.m_handle);
			Logger::print(encrypt("Found base address:\t0x%llx\n"), m_application.m_base_address);

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
#else
			return Interface.Read<T>(addr);
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
#else
			if (Interface.Read(addr, buffer, size))
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
#else
			if (Interface.Write<T>(addr, value))
				return true;
#endif

			return false;
		}
	};
}

External::Runtime::c_memory g_memory{};

#undef is_invalid