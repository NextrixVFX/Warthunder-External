#pragma once

namespace External::Engine::SDK
{
	enum e_unit_state : int8_t
	{
		m_alive,
		m_damaged,
		m_dead
	};

	enum e_unit_type : int8_t
	{
		m_plane,
		m_idk1,
		m_idk2,
		m_tank
	};

	std::string read_string(uintptr_t addr, size_t max_len = 64)
	{
		if (!g_memory.is_valid_address(addr))
			return {};

		std::string buffer(max_len, '\0');

		g_memory.read(addr, buffer.data(), max_len);

		buffer.back() = '\0';

		return std::string(buffer.c_str());
	}

	std::string read_player_name(uintptr_t player_info)
	{
		std::string unknown = encrypt("Unknown");

		uintptr_t name_ptr =
			g_memory.read<uintptr_t>(player_info + offsets::player::m_name);

		if (!g_memory.is_valid_address(name_ptr))
			return unknown;

		std::string name = read_string(name_ptr, 64);

		if (name.empty())
			return unknown;

		size_t slash = name.find_last_of("/\\");
		if (slash != std::string::npos)
			name = name.substr(slash + 1);

		size_t ext = name.find('.');
		if (ext != std::string::npos)
			name = name.substr(0, ext);

		std::replace(name.begin(), name.end(), '_', ' ');

		return name.empty() ? unknown : name;
	}
}