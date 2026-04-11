#pragma once

namespace External::Runtime
{
	struct entity_t
	{
		uintptr_t m_base = 0;
		int8_t m_team_num = 0;
		int8_t m_unit_type = 0;
		int8_t m_unit_state = 0;
		float m_distance = 0.f;
		std::string m_username{};
		vector3_t<float> m_position{};
		matrix3x3_t<float> m_rotation_matrix{};
		Engine::Structs::bbox_t m_bbox{};
		std::array<vector2_t<float>, 8> m_screen_coords{};
	};
	
	struct entities_frame_t
	{
		std::vector<entity_t> m_entities{};
	};

	class c_entities_cache : public c_cache
	{
	private:
		static inline c_cache_registrar<c_entities_cache> m_registrar{};
		static inline c_engine_cache* m_engine_cache = 0;
		static inline c_camera_cache* m_camera_cache = 0;

	public:
		c_double_buffer<entities_frame_t> m_data{};

		c_entities_cache() : c_cache(CACHE_ID::entities) {}

		void tick() override
		{
			entities_frame_t frame{};

			c_engine_cache::get(m_engine_cache);
			auto& engine_data = m_engine_cache->m_data.get();

			c_camera_cache::get(m_camera_cache);
			auto& camera_data = m_camera_cache->m_data.get();

			float closest_distance = FLT_MAX;
			uintptr_t closest_entity = 0;
			bool got_closest_entity = false;

			if (g_memory.is_valid_address((uintptr_t)engine_data.m_unit_list_3.m_data))
			{
				uintptr_t arr_base = (uintptr_t)engine_data.m_unit_list_3.m_data;
				for (size_t idx{ 0 }; idx < engine_data.m_unit_list_3.m_count; ++idx)
				{
					entity_t ent{};
					ent.m_base = g_memory.read<uintptr_t>(arr_base + idx * sizeof(uintptr_t));
					if (!g_memory.is_valid_address(ent.m_base))
						continue;

					uintptr_t player_info = g_memory.read<uintptr_t>(ent.m_base + offsets::unit::m_player_info);
					if (!g_memory.is_valid_address(player_info))
						continue;

					ent.m_username = Engine::SDK::read_player_name(player_info);
					ent.m_team_num = g_memory.read<int8_t>(ent.m_base + offsets::unit::m_team_num);
					ent.m_unit_state = g_memory.read<int8_t>(ent.m_base + offsets::unit::m_unit_state);
					ent.m_unit_type = g_memory.read<int8_t>(ent.m_base + offsets::unit::m_unit_type);
					ent.m_position = g_memory.read<vector3_t<float>>(ent.m_base + offsets::unit::m_position);
					ent.m_rotation_matrix = g_memory.read<matrix3x3_t<float>>(ent.m_base + offsets::unit::m_rotation_matrix);
					ent.m_bbox.m_min = g_memory.read<vector3_t<float>>(ent.m_base + offsets::unit::m_bb_min);
					ent.m_bbox.m_max = g_memory.read<vector3_t<float>>(ent.m_base + offsets::unit::m_bb_max);
					ent.m_screen_coords = ent.m_bbox.get_screen_coords(camera_data.m_camera_data, ent.m_position, ent.m_rotation_matrix);

					if (ent.m_base == global::g_local.m_local_player.m_unit_ptr)
						global::g_local.m_local_player.m_team_idx = ent.m_team_num;

					ent.m_distance = ent.m_position.Distance(camera_data.m_camera_data.m_positon);
					if (ent.m_distance < closest_distance)
					{
						closest_distance = ent.m_distance;
						closest_entity = ent.m_base;
						got_closest_entity = true;
					}
					
					frame.m_entities.emplace_back(ent);
				}

				global::g_local.m_local_player.m_unit_ptr = (got_closest_entity) ? closest_entity : 0;
			}

			m_data.update(frame);
			sleep_short(1);
		}

		template<typename T = c_entities_cache*>
		static void get(T*& ptr)
		{
			if (!ptr)
				ptr = reinterpret_cast<T*>(get_cache(CACHE_ID::entities));
		}
	};
}