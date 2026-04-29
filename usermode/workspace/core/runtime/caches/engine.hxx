#pragma once

namespace External::Runtime
{
	struct unit_list_t
	{
		uintptr_t* m_data;        // 0x0
		uint8_t   pad0[0x8];	 // stride
		uint32_t  m_count;       // 0x10
		uint8_t   pad1[0x4];     // stride
	};

	struct s_engine_frame
	{
		uintptr_t m_game_context = 0;
		uintptr_t m_local_player = 0;
		uintptr_t m_camera_controller = 0;
		//unit_list_t m_unit_list_1{};
		//unit_list_t m_unit_list_2{};
		unit_list_t m_unit_list_3{};
		Engine::Structs::camera_t m_camera_data{};
	};

	class c_engine_cache : public c_cache
	{
	private:
		static inline c_cache_registrar<c_engine_cache> m_registrar{};

	public:
		c_double_buffer<s_engine_frame> m_data{};
		c_engine_cache() : c_cache(CACHE_ID::engine) {}

		void tick() override
		{
			sleep_short(4);

			s_engine_frame frame{};
			uintptr_t base_address = g_memory.m_application.m_base_address;
			
			frame.m_game_context = g_memory.read<uintptr_t>(base_address + offsets::global::g_game_context);
			if (!g_memory.is_valid_address(frame.m_game_context))
			{
				return;
			}
			
			frame.m_local_player = g_memory.read<uintptr_t>(base_address + offsets::global::g_local_player);
			if (!g_memory.is_valid_address(frame.m_local_player))
			{
				return;
			}

			frame.m_camera_controller = g_memory.read<uintptr_t>(frame.m_game_context + offsets::game::g_camera_controller);
			if (!g_memory.is_valid_address(frame.m_camera_controller))
			{
				return;
			}

			frame.m_unit_list_3 = g_memory.read<unit_list_t>(frame.m_game_context + offsets::game::g_unit_list_3);
			
			m_data.update(frame);
		}

		template<typename T = c_engine_cache*>
		static void get(T*& ptr)
		{
			if (!ptr)
				ptr = reinterpret_cast<T*>(get_cache(CACHE_ID::engine));
		}
	};
}