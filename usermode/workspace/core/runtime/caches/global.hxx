#pragma once

namespace global
{
	struct local_player_t
	{
		uintptr_t m_unit_ptr = 0;
		int m_team_idx = -1;
	};

	struct local_t
	{
		local_player_t m_local_player{};

	} g_local;
}