#pragma once

namespace External::Render
{
	class c_shapes
	{
	private:
		enum eDrawFlags { drop_shadow, outline };
		ImDrawList* m_draw_list{ };

	public:
		inline void set_draw_list(ImDrawList* draw_list)
		{
			m_draw_list = draw_list;
		}

		inline auto& get_draw_list()
		{
			return m_draw_list;
		}

		inline auto Text(const ImVec2& txt_pos, const ImU32& color, const int flags,
			const std::string_view& message, ImFont* font, bool centered = true) const -> void
		{
			const auto size = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, message.data());
			auto pos = ImVec2(txt_pos.x, txt_pos.y);

			if (centered)
				pos.x -= size.x / 2.0f;

			ImColor outline_clr = ImColor(1, 1, 1, 255);

			if (flags & drop_shadow)
				m_draw_list->AddText(font, font->FontSize, ImVec2(pos.x + 1, pos.y + 1), outline_clr, message.data());

			if (flags & outline) [[likely]]
			{
				static const ImVec2 offsets[8] = {
					{ 1, -1 }, { -1, 1 }, { -1, -1 }, { 1, 1 },
					{ 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 }
				};

				for (const ImVec2& offset : offsets)
					m_draw_list->AddText(font, font->FontSize, ImVec2(pos.x + offset.x, pos.y + offset.y), outline_clr, message.data());
			}

			m_draw_list->AddText(font, font->FontSize, pos, color, message.data());
		}

		inline auto Image(float x1, float y1, float x2, float y2,
			ID3D11ShaderResourceView* tex, float rounding = 0.0f, ImColor col = { 255,255,255,255 }) const -> void
		{
			m_draw_list->AddImageRounded((ImTextureID)(intptr_t)tex, { x1, y1 }, { x2, y2 }, { 0, 0 }, { 1, 1 }, col, rounding);
		}

		inline auto Line(float x1, float y1, float x2, float y2, const ImU32& clr, float thickness = 1.0f) const -> void
		{
			m_draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), clr, thickness);
		}

		inline auto Curve(float x1, float y1, float x2, float y2, const ImU32& clr, float thickness = 1.0f, float curve = 1.0f) const -> void
		{
			float curveAmnt = std::abs(x2 - x1) * curve;
			m_draw_list->AddBezierCubic(ImVec2(x1, y1), ImVec2(x1, y1 + curveAmnt), ImVec2(x2, y2 - curveAmnt), ImVec2(x2, y2), clr, thickness);
		}

		inline auto Rect(float x, float y, float w, float h, const ImU32& clr, float rounding = 0.0f, float thickness = 1.0f) const -> void
		{
			m_draw_list->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), clr, rounding, 1, thickness);
		}

		inline auto CornerRect(float X, float Y, float W, float H, const ImU32& color, float thickness = 1.0f) const -> void
		{
			float lineW = (W / 3);
			float lineH = (H / 4);

			Line(X, Y, X, Y + lineH, color, thickness);
			Line(X, Y, X + lineW, Y, color, thickness);
			Line(X + W - lineW, Y, X + W, Y, color, thickness);
			Line(X + W, Y, X + W, Y + lineH, color, thickness);
			Line(X, Y + H - lineH, X, Y + H, color, thickness);
			Line(X, Y + H, X + lineW, Y + H, color, thickness);
			Line(X + W - lineW, Y + H, X + W, Y + H, color, thickness);
			Line(X + W, Y + H - lineH, X + W, Y + H, color, thickness);
		}

		inline auto FilledRect(float x, float y, float w, float h, const ImU32& color, float rounding, int flags) const -> void
		{
			m_draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding, flags);
		}

		inline auto FilledRectGradient(float x, float y, float w, float h, const ImU32& col_upr_left, const ImU32& col_upr_right, const ImU32& col_bot_right, const ImU32& col_bot_left) const -> void
		{
			m_draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), col_upr_left, col_upr_right, col_bot_right, col_bot_left);
		}

		inline auto Triangle(float x1, float y1, float x2, float y2, float x3, float y3, const ImU32& clr, float thickness) const -> void
		{
			m_draw_list->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr, thickness);
		}

		inline auto TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, const ImU32& clr) const -> void
		{
			m_draw_list->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr);
		}

		inline auto Circle(float x1, float y1, float radius, const ImU32& col, int segments) const -> void
		{
			m_draw_list->AddCircle(ImVec2(x1, y1), radius, col, segments);
		}

		inline auto CircleFilled(float x1, float y1, float radius, const ImU32& col, int segments) const -> void
		{
			m_draw_list->AddCircleFilled(ImVec2(x1, y1), radius, col, segments);
		}
	};

	std::unique_ptr<c_shapes> inst_shapes = std::make_unique<c_shapes>();
}