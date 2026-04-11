#pragma once

namespace External::Features::Visual
{
	using namespace External::Runtime;

	class c_esp : public c_feature
	{
	private:
		static inline c_feature_registrar<c_esp> m_registrar{};
		static inline c_camera_cache* m_camera_cache = 0;
		static inline c_entities_cache* m_entity_cache = 0;

		static constexpr std::array<std::pair<int, int>, 12> edges = { {
			{0,1}, {1,2}, {2,3}, {3,0}, // bottom
			{4,5}, {5,6}, {6,7}, {7,4}, // top
			{0,4}, {1,5}, {2,6}, {3,7}  // vertical
		} };

	public:
		c_esp() : c_feature(FEATURE_ID::visual_esp) {}

		void on_render() override
		{
			ImFont* font = Render::c_render::m_fonts.font_16;
			
			c_camera_cache::get(m_camera_cache);
			auto& cam_data = m_camera_cache->m_data.get().m_camera_data;

			c_entities_cache::get(m_entity_cache);
			const auto& entity_data = m_entity_cache->m_data.get().m_entities;
			if (entity_data.empty())
				return;

			for (const auto& entity : entity_data)
			{
				ImColor entity_color = Config::g_tank_color;
				
				if (entity.m_unit_state == Engine::SDK::e_unit_state::m_dead)
					continue;

				if (entity.m_unit_type == Engine::SDK::e_unit_type::m_plane)
					entity_color = Config::g_plane_color;

				if (entity.m_team_num == global::g_local.m_local_player.m_team_idx)
					entity_color = Config::g_teammate_color;

				if (Config::g_boxes_enabled)
				{
					for (const auto& [i1, i2] : edges)
					{
						const auto& p1 = entity.m_screen_coords[i1];
						const auto& p2 = entity.m_screen_coords[i2];

						if (p1.x == -1 || p2.x == -1)
							continue;

						Render::inst_shapes->Line(
							p1.x, p1.y,
							p2.x, p2.y,
							entity_color,
							Config::g_box_thickness
						);
					}
				}
				
				vector2_t<float> base_pos = cam_data.project_to_screen(entity.m_position);
				if (base_pos.x == -1)
					continue;

				float offset = base_pos.y / 32.0f;

				if (Config::g_lines_enabled && entity.m_team_num != global::g_local.m_local_player.m_team_idx)
				{
					Render::inst_shapes->Line(
						screen_center.x, screen_height * Config::g_lines_origin,
						base_pos.x, base_pos.y + offset,
						entity_color,
						Config::g_line_thickness
					);
				}

				if (Config::g_usernames_enabled)
				{
					auto color = ImColor{ 255,255,255,255 };
					Render::inst_shapes->Text({ base_pos.x, base_pos.y + offset }, color, 1, entity.m_username, font);
				}
				
				if (Config::g_distance_enabled)
				{
					const float dist_meters = entity.m_distance * 1.0f;
					const float normalized_dist = std::clamp<float>(1.0f - (dist_meters / 600.0f), 0.0f, 1.0f);
					std::string dist_str = std::format("{:.1f}m", dist_meters);
					ImColor col = ImLerp(ImColor(0, 255, 0, 255).Value, ImColor(255, 0, 0, 255).Value, normalized_dist);
					Render::inst_shapes->Text(
						{ base_pos.x, base_pos.y + (offset + Config::g_usernames_enabled ? 32.f : 0.f) }, col, 1, dist_str, font);
				}
			}
		}

		void on_update() override
		{
			
		}
	};
}