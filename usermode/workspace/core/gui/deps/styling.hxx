#pragma once

namespace External::Render
{
	class c_styling
	{
	private:
		inline static float m_ui_scale{ 1.f };

	public:
		inline static Assets::fonts_t m_fonts{};

		inline static ImVec2 m_menu_base_size{ 640.f, 380.f };
		inline static ImVec2 m_menu_min_size{ 520.f, 300.f };
		inline static float m_menu_rounding{ 6.f };
		inline static float m_child_rounding{ 4.f };
		inline static float m_page_rounding{ 5.f };
		inline static float m_elements_rounding{ 4.f };

		inline static ImColor m_dark_color{ 15, 16, 20, 255 };
		inline static ImColor m_second_color{ 17, 18, 22, 255 };
		inline static ImColor m_background_color{ 22, 30, 25, 255 };
		inline static ImColor m_stroke_color{ 38, 42, 52, 255 };
		inline static ImColor m_window_bg_color{ 17, 18, 22, 245 };
		inline static ImColor m_separator_color{ 38, 42, 52, 255 };

		inline static ImColor m_child_background{ 17, 18, 22, 255 };
		inline static ImColor m_child_stroke{ 38, 42, 52, 155 };

		inline static ImColor m_page_background{ 15, 16, 20, 255 };
		inline static ImColor m_page_background_active{ 17, 18, 22, 255 };
		inline static ImColor m_page_text{ 235, 239, 241, 255 };
		inline static ImColor m_page_text_hovered{ 21, 37, 35, 255 };

		inline static ImColor m_elements_background{ 17, 18, 22, 255 };
		inline static ImColor m_elements_background_hovered{ 38, 42, 52, 255 };

		inline static ImColor m_label_active{ 240, 240, 255, 255 };
		inline static ImColor m_label_hovered{ 220, 220, 240, 255 };
		inline static ImColor m_label_default{ 151, 155, 176, 255 };

		inline static ImColor m_description_active{ 180, 180, 180, 255 };
		inline static ImColor m_description_hovered{ 21, 37, 35, 255 };
		inline static ImColor m_description_default{ 151, 155, 176, 255 };

		inline static ImVec4 m_section_title{ 0.93f, 0.93f, 0.97f, 1.0f };
		inline static ImVec4 m_widget_label{ 0.74f, 0.74f, 0.79f, 1.0f };
		inline static ImVec4 m_widget_label_disabled{ 0.38f, 0.38f, 0.42f, 1.0f };
		inline static ImVec4 m_value_text{ 0.62f, 0.62f, 0.67f, 1.0f };
		inline static ImVec4 m_value_text_disabled{ 0.42f, 0.42f, 0.46f, 1.0f };

		static void set_scale(float scale)
		{
			if (scale < 0.75f)
				scale = 0.75f;
			if (scale > 2.5f)
				scale = 2.5f;

			m_ui_scale = scale;
		}

		static float get_scale()
		{
			return m_ui_scale;
		}

		static float apply_scale(float v)
		{
			return v * m_ui_scale;
		}

		static ImVec2 apply_scale(float x, float y)
		{
			return { x * m_ui_scale, y * m_ui_scale };
		}

		static ImVec2 apply_scale(const ImVec2& v)
		{
			return { v.x * m_ui_scale, v.y * m_ui_scale };
		}

		static ImVec2 menu_size()
		{
			return apply_scale(m_menu_base_size);
		}

		static ImVec2 menu_min_size()
		{
			return apply_scale(m_menu_min_size);
		}

		static float menu_rounding()
		{
			return apply_scale(m_menu_rounding);
		}

		static float child_rounding()
		{
			return apply_scale(m_child_rounding);
		}

		static float page_rounding()
		{
			return apply_scale(m_page_rounding);
		}

		static float elements_rounding()
		{
			return apply_scale(m_elements_rounding);
		}

		static ImVec4 accent()
		{
			return Config::g_menu_accent_color.Value;
		}
	};

	inline c_styling g_styling{};
}
