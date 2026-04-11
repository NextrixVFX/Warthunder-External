#pragma once
#include "imgui.h"
#include <string>
#include <algorithm>
#include <string>
#include <cctype>
#include <imgui_internal.h>

namespace font
{
	inline ImFont* icomoon_logo = nullptr;
	inline ImFont* description_font = nullptr;
	inline ImFont* esp_font = nullptr;
	inline ImFont* tab = nullptr;
	inline ImFont* regular_m = nullptr;
	inline ImFont* regular_l = nullptr;
	inline ImFont* icomoon_page = nullptr;
	inline ImFont* small_font = nullptr;
	inline ImFont* inter_semibold = nullptr;
	inline ImFont* s_inter_semibold = nullptr;
	inline ImFont* inter_medium = nullptr;
	inline ImFont* icon_notify = nullptr;
	inline ImFont* bold_font = nullptr;
	inline ImFont* font_black = nullptr;
}

namespace utils
{
	inline float CalculateTextWidthWithoutColorCodes(const ImFont* font, const char* text_begin, const char* text_end = nullptr) {
		if (!text_end)
			text_end = text_begin + strlen(text_begin);

		float text_width = 0.0f;
		const char* s = text_begin;

		while (s < text_end) {
			if (*s == '^' && (s + 1) < text_end && *(s + 1) >= '0' && *(s + 1) <= '9') {
				s += 2;
				continue;
			}

			unsigned int c = (unsigned int)*s;
			if (c < 0x80) {
				s += 1;
			}
			else {
				s += ImTextCharFromUtf8(&c, s, text_end);
				if (c == 0)
					break;
			}

			const ImFontGlyph* glyph = font->FindGlyph((ImWchar)c);
			if (glyph) {
				text_width += glyph->AdvanceX;
			}
		}

		return text_width;
	}

	inline ImColor GetColorWithAlpha(ImColor color, float alpha)
	{
		return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
	}

	inline ImVec2 center_text(ImVec2 min, ImVec2 max, const char* text)
	{
		return min + (max - min) / 2 - ImGui::CalcTextSize(text) / 2;
	}

	inline ImColor GetDarkColor(const ImColor& color)
	{
		float r, g, b, a;
		r = color.Value.x;
		g = color.Value.y;
		b = color.Value.z;
		a = 255;

		float darkPercentage = 0.2f;
		float darkR = r * darkPercentage;
		float darkG = g * darkPercentage;
		float darkB = b * darkPercentage;

		return ImColor(darkR, darkG, darkB, a);
	}
	inline ImVec4 ImColorToImVec4(const ImColor& color)
	{
		return ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w);
	}

}

inline std::string selected_category = "COMBAT";

inline char search_input[50] = { "" };
inline bool search_active;

// Приводим строку в нижний регистр (без UB на знаковом char)
static std::string ToLower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
		});
	return s;
}

// Проверяем содержание без учёта регистра
inline bool ContainsCaseInsensitive(const std::string& haystack, const std::string& needle) {
	if (needle.empty())
		return true;               // пустая «игнорируется»
	auto hay = ToLower(haystack);
	auto ned = ToLower(needle);
	return hay.find(ned) != std::string::npos;
}


inline bool tab_is_changed = true;
inline float tab_offset = 0.f;
inline int stored_tab = 0;
inline std::string stored_category = "COMBAT";

inline namespace c
{
	inline ImColor dark_color(15, 16, 20, 255);
	inline ImColor second_color(17, 18, 22, 255);
	inline ImColor background_color(22, 30, 25, 255);
	inline ImColor stroke_color(38, 42, 52, 255);
	inline ImColor window_bg_color(17, 18, 22, 245);

	inline ImVec4 accent = ImColor(0, 250, 180, 255);
	inline ImVec4 separator = ImColor(38, 42, 52, 255);

	namespace anim
	{
		inline float  speed = 0.3f;
		inline ImColor active = ImColor(0, 250, 180, 255);
		inline ImColor ddefault = ImColor(22, 30, 35, 255);
	}

	namespace bg
	{
		inline ImColor background = ImColor(21, 37, 35, 255);
		inline ImVec2 size = ImVec2(850, 650);
		inline float   rounding = 9.f;
	}

	namespace child
	{
		inline ImColor background = ImColor(17, 18, 22, 255);
		inline ImColor stroke = ImColor(38, 42, 52, 155);
		inline float   rounding = 7.f;
	}

	namespace page
	{
		inline ImColor background_active = ImColor(17, 18, 22, 255);
		inline ImColor background = ImColor(15, 16, 20, 255);
		inline ImColor text_hov = ImColor(21, 37, 35, 255);
		inline ImColor text = ImColor(235, 239, 241, 255);
		inline float   rounding = 5.f;
	}

	namespace elements
	{
		inline ImColor background_hovered = ImColor(38, 42, 52, 255);
		inline ImColor background = ImColor(17, 18, 22, 255);
		inline float   rounding = 4.f;
	}

	namespace checkbox
	{
		inline ImColor mark = ImColor(21, 37, 35, 255);
	}

	inline namespace text
	{
		namespace label
		{
			inline ImColor active = ImColor(240, 240, 255, 255);
			inline ImColor hovered = ImColor(220, 220, 240, 255);
			inline ImColor ddefault = ImColor(151, 155, 176, 255);
		}

		namespace description
		{
			inline ImColor active = ImColor(180, 180, 180, 255);
			inline ImColor hovered = ImColor(21, 37, 35, 255);
			inline ImColor ddefault = ImColor(151, 155, 176, 255);
		}
	}
}
