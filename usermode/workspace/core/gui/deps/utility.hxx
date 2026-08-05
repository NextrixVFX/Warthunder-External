#pragma once

namespace External::Render
{
	class c_utility
	{
	private:
	public:
		static ImVec2 center_text(ImVec2 min, ImVec2 max, std::string_view text)
		{
			return min + (max - min) / 2 - ImGui::CalcTextSize(text.data()) / 2;
		}

		static ImColor set_alpha(ImColor color, float alpha)
		{
			return { color.Value.x, color.Value.y, color.Value.z, alpha };
		}

		static ImColor multiply_alpha(ImColor color, float alpha)
		{
			return { color.Value.x, color.Value.y, color.Value.z, color.Value.w * alpha };
		}

		static ImColor get_dark_color(const ImColor& color, float factor = 0.2f)
		{
			float r = color.Value.x;
			float g = color.Value.y;
			float b = color.Value.z;
			return { r * factor, g * factor, b * factor, color.Value.w };
		}
	};

	inline c_utility g_utility{};
}