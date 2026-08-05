#pragma once

using namespace ImGui;

inline static std::unordered_map<ImGuiID, float> g_checkboxAnim;
inline static std::unordered_map<ImGuiID, float> s_tabAnim;
inline static std::unordered_map<ImGuiID, float> g_sliderAnim;
inline static std::unordered_map<ImGuiID, float> g_comboAnim;
inline static std::unordered_map<ImGuiID, bool> g_keybindListening;
inline static std::unordered_map<ImGuiID, bool> g_sectionOpen;
inline static std::unordered_map<ImGuiID, float> g_sectionCollapseAnim;
inline static std::unordered_map<ImGuiID, float> g_sectionContentHeight;

namespace External::Render::Widgets
{
    namespace Layout
    {
        inline float kSectionPadH() { return Render::g_styling.apply_scale(12.0f); }
        inline float kSectionPadV() { return Render::g_styling.apply_scale(4.0f); }
        inline float kSectionGap() { return Render::g_styling.apply_scale(10.0f); }
        inline float kSectionTitleGap() { return Render::g_styling.apply_scale(4.0f); }
        inline float kSectionHeaderH() { return Render::g_styling.apply_scale(30.0f); }
        inline float kSectionRounding() { return Render::g_styling.apply_scale(8.0f); }
        inline float kColumnMargin() { return Render::g_styling.apply_scale(4.0f); }
        inline float kRowHeight() { return Render::g_styling.apply_scale(28.0f); }
        inline float kControlHeight() { return Render::g_styling.apply_scale(22.0f); }
        inline float kItemSpacingY() { return Render::g_styling.apply_scale(5.0f); }
        inline float kWidgetGap() { return Render::g_styling.apply_scale(3.0f); }
        inline float kLabelControlGap() { return Render::g_styling.apply_scale(7.0f); }
        inline float kSliderLabelGap() { return Render::g_styling.apply_scale(3.0f); }
        inline float kToggleWidth() { return Render::g_styling.apply_scale(40.0f); }

        inline float ContentWidth()
        {
            return ImMax(
                Render::g_styling.apply_scale(72.0f),
                ImGui::GetColumnWidth() - kSectionPadH() * 2.0f - kColumnMargin()
            );
        }

        inline float SectionWidth()
        {
            return ImMax(
                Render::g_styling.apply_scale(96.0f),
                ImGui::GetColumnWidth() - kColumnMargin() * 2.0f
            );
        }
    }

    namespace Colors
    {
        inline ImVec4 SectionTitle() { return Render::g_styling.m_section_title; }
        inline ImVec4 WidgetLabel() { return Render::g_styling.m_widget_label; }
        inline ImVec4 WidgetLabelDisabled() { return Render::g_styling.m_widget_label_disabled; }
        inline ImVec4 ValueText() { return Render::g_styling.m_value_text; }
        inline ImVec4 ValueTextDisabled() { return Render::g_styling.m_value_text_disabled; }
    }

    inline static ImVec4 accent()
    {
        return Render::g_styling.accent();
    }

    inline static ImVec4 mul(ImVec4 c, float v, float a = 1.0f)
    {
        return ImVec4(
            ImClamp(c.x * v, 0.0f, 1.0f),
            ImClamp(c.y * v, 0.0f, 1.0f),
            ImClamp(c.z * v, 0.0f, 1.0f),
            a
        );
    }

    inline static ImU32 col(ImVec4 c)
    {
        return ImGui::ColorConvertFloat4ToU32(c);
    }

    inline static float accent_alpha()
    {
        return ImClamp(accent().w, 0.0f, 1.0f);
    }

    inline static ImVec4 accent_bg(float rgb_mul, float alpha_mul)
    {
        const ImVec4 acc = accent();
        return ImVec4(
            ImClamp(acc.x * rgb_mul, 0.0f, 1.0f),
            ImClamp(acc.y * rgb_mul, 0.0f, 1.0f),
            ImClamp(acc.z * rgb_mul, 0.0f, 1.0f),
            ImClamp(alpha_mul * accent_alpha(), 0.0f, 1.0f)
        );
    }

    inline static void DrawAccentGradientLine(ImDrawList* draw_list, ImVec2 min, ImVec2 max, float alpha_mul = 1.0f)
    {
        const ImVec4 acc = accent();
        const float a = accent_alpha() * alpha_mul;
        const ImU32 left_col = col(ImVec4(acc.x, acc.y, acc.z, a));
        const ImU32 right_col = col(ImVec4(acc.x, acc.y, acc.z, a * 0.01f));

        draw_list->AddRectFilledMultiColor(min, max, left_col, right_col, right_col, left_col);
    }

    inline static bool IsWidgetDisabled(bool explicit_disabled = false)
    {
        return explicit_disabled || (GImGui->CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
    }

    inline static ImU32 WidgetLabelCol(bool disabled = false)
    {
        return col(IsWidgetDisabled(disabled) ? Colors::WidgetLabelDisabled() : Colors::WidgetLabel());
    }

    inline static ImU32 ValueTextCol(bool disabled = false)
    {
        return col(IsWidgetDisabled(disabled) ? Colors::ValueTextDisabled() : Colors::ValueText());
    }

    inline static void DrawLabel(const ImVec2& pos, const char* text, bool disabled = false)
    {
        ImGui::GetWindowDrawList()->AddText(pos, WidgetLabelCol(disabled), text);
    }

    inline static void DrawSectionCard(ImDrawList* draw_list, ImVec2 min, ImVec2 max, float rounding)
    {
        const ImVec4 acc = accent();

        draw_list->AddRectFilled(
            min,
            max,
            col(accent_bg(0.14f, 0.42f)),
            rounding
        );

        draw_list->AddRect(
            min,
            max,
            col(ImVec4(acc.x, acc.y, acc.z, 0.22f * accent_alpha())),
            rounding,
            0,
            1.0f
        );
    }

    inline static void DrawParallaxBackgroundGlows(const ImVec2& region_min, const ImVec2& region_max, const ImVec4& accent_col)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();

        static ImVec2 parallax(0.0f, 0.0f);
        const ImVec2 region_center(
            (region_min.x + region_max.x) * 0.5f,
            (region_min.y + region_max.y) * 0.5f
        );

        if (io.MousePos.x >= 0.0f && io.MousePos.y >= 0.0f)
        {
            ImVec2 target(
                (io.MousePos.x - region_center.x) * 0.045f,
                (io.MousePos.y - region_center.y) * 0.045f
            );
            
            const float smooth = ImClamp(io.DeltaTime * 7.0f, 0.0f, 1.0f);
            parallax.x += (target.x - parallax.x) * smooth;
            parallax.y += (target.y - parallax.y) * smooth;
        }

        const float region_w = region_max.x - region_min.x;
        const float region_h = region_max.y - region_min.y;

        struct GlowSpec
        {
            ImVec2 norm_pos;
            float radius;
            int segments;
            float alpha;
            float parallax_mul;
        };

        const GlowSpec glows[] = {
            { ImVec2(0.18f, 0.22f), Render::g_styling.apply_scale(52.0f), 6, 0.11f, -2.00f },
            { ImVec2(0.82f, 0.30f), Render::g_styling.apply_scale(64.0f), 3, 0.09f, -0.85f },
            { ImVec2(0.72f, 0.78f), Render::g_styling.apply_scale(48.0f), 8, 0.08f, 1.35f },
            { ImVec2(0.28f, 0.68f), Render::g_styling.apply_scale(40.0f), 5, 0.10f, -1.45f },
            { ImVec2(0.50f, 0.48f), Render::g_styling.apply_scale(36.0f), 7, 0.06f, 2.70f },
        };

        draw_list->PushClipRect(region_min, region_max, true);

        for (const GlowSpec& glow : glows)
        {
            const ImVec2 center(
                region_min.x + region_w * glow.norm_pos.x + parallax.x * glow.parallax_mul,
                region_min.y + region_h * glow.norm_pos.y + parallax.y * glow.parallax_mul
            );

            draw_list->AddShadowNGon(
                center,
                glow.radius * 0.25f,
                ImGui::ColorConvertFloat4ToU32(ImVec4(accent_col.x, accent_col.y, accent_col.z, glow.alpha * 6.f)),
                glow.radius * 6.f,
                ImVec2(0.0f, 0.0f),
                0,
                glow.segments
            );
        }

        draw_list->PopClipRect();
    }

    inline static void DrawChevron(ImDrawList* draw_list, ImVec2 center, float size, float rotation, ImU32 color)
    {
        const float half = size * 0.5f;
        ImVec2 points[3] = {
            ImVec2(-half * 0.55f, -half * 0.45f),
            ImVec2(half * 0.55f, -half * 0.45f),
            ImVec2(0.0f, half * 0.50f)
        };

        const float cos_a = cosf(rotation);
        const float sin_a = sinf(rotation);
        ImVec2 rotated[3];
        for (int i = 0; i < 3; ++i)
        {
            rotated[i] = ImVec2(
                center.x + points[i].x * cos_a - points[i].y * sin_a,
                center.y + points[i].x * sin_a + points[i].y * cos_a
            );
        }

        draw_list->AddTriangleFilled(rotated[0], rotated[1], rotated[2], color);
    }

    inline static float AnimateToward(ImGuiID id, float target, float speed, std::unordered_map<ImGuiID, float>& map)
    {
        auto it = map.find(id);
        if (it == map.end())
            it = map.emplace(id, target).first;

        float& value = it->second;
        value += (target - value) * ImClamp(ImGui::GetIO().DeltaTime * speed, 0.0f, 1.0f);
        if (fabsf(target - value) < 0.001f)
            value = target;

        return value;
    }

    inline static const char* VkToString(int vk)
    {
        switch (vk)
        {
        case VK_LBUTTON: return "LMB";
        case VK_RBUTTON: return "RMB";
        case VK_MBUTTON: return "MMB";
        case VK_XBUTTON1: return "X1";
        case VK_XBUTTON2: return "X2";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_SPACE: return "Space";
        case VK_TAB: return "Tab";
        case VK_ESCAPE: return "Esc";
        case VK_DELETE: return "Del";
        case VK_INSERT: return "Ins";
        case VK_HOME: return "Home";
        case VK_END: return "End";
        case VK_PRIOR: return "PgUp";
        case VK_NEXT: return "PgDn";
        case VK_LEFT: return "Left";
        case VK_RIGHT: return "Right";
        case VK_UP: return "Up";
        case VK_DOWN: return "Down";
        default: break;
        }

        static char buf[16];
        if (vk >= 'A' && vk <= 'Z')
        {
            buf[0] = (char)vk;
            buf[1] = '\0';
            return buf;
        }
        if (vk >= '0' && vk <= '9')
        {
            buf[0] = (char)vk;
            buf[1] = '\0';
            return buf;
        }
        if (vk >= VK_F1 && vk <= VK_F12)
        {
            snprintf(buf, sizeof(buf), "F%d", vk - VK_F1 + 1);
            return buf;
        }

        snprintf(buf, sizeof(buf), "0x%02X", vk);
        return buf;
    }

    inline static void BeginPanelColumns(const char* id)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Render::g_styling.apply_scale(8.0f), Layout::kItemSpacingY()));
        ImGui::Columns(2, id, false);
    }

    inline static void EndPanelColumns()
    {
        ImGui::Columns(1);
        ImGui::PopStyleVar();
    }

    inline static bool BeginSection(const char* title, bool default_open = true)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = window->DrawList;

        ImGui::PushID(title);

        const ImGuiID section_id = ImGui::GetID("##section");
        auto open_it = g_sectionOpen.find(section_id);
        if (open_it == g_sectionOpen.end())
            open_it = g_sectionOpen.emplace(section_id, default_open).first;

        bool& open = open_it->second;
        const float collapse_anim = AnimateToward(
            section_id,
            open ? 1.0f : 0.0f,
            16.0f,
            g_sectionCollapseAnim
        );

        draw_list->ChannelsSplit(2);
        draw_list->ChannelsSetCurrent(1);

        const float card_w = Layout::SectionWidth();
        const float header_h = Layout::kSectionHeaderH();
        const ImVec2 start_pos = ImGui::GetCursorScreenPos();

        ImGui::GetStateStorage()->SetFloat(ImGui::GetID("##sec_start_y"), start_pos.y);
        ImGui::GetStateStorage()->SetFloat(ImGui::GetID("##sec_start_x"), start_pos.x);
        ImGui::GetStateStorage()->SetFloat(ImGui::GetID("##sec_header_h"), header_h);
        ImGui::GetStateStorage()->SetFloat(ImGui::GetID("##sec_collapse_anim"), collapse_anim);

        const ImRect header_bb(start_pos, start_pos + ImVec2(card_w, header_h));
        const ImGuiID header_id = ImGui::GetID("##header");

        ImGui::ItemSize(header_bb);
        if (ImGui::ItemAdd(header_bb, header_id))
        {
            bool header_hovered = false;
            bool header_held = false;
            if (ImGui::ButtonBehavior(header_bb, header_id, &header_hovered, &header_held))
                open = !open;

            const float chevron_rot = ImLerp(-1.5707963f, 0.0f, collapse_anim);
            const ImVec2 chevron_center(
                header_bb.Min.x + Layout::kSectionPadH() + Render::g_styling.apply_scale(5.0f),
                header_bb.Min.y + header_h * 0.5f
            );
            DrawChevron(
                draw_list,
                chevron_center,
                Render::g_styling.apply_scale(7.0f),
                chevron_rot,
                col(header_hovered ? Colors::SectionTitle() : ImVec4(0.68f, 0.68f, 0.73f, 1.0f))
            );

            const ImVec2 title_pos(
                header_bb.Min.x + Layout::kSectionPadH() + Render::g_styling.apply_scale(16.0f),
                header_bb.Min.y + (header_h - ImGui::CalcTextSize(title).y) * 0.5f
            );
            const ImVec2 title_size = ImGui::CalcTextSize(title);

            draw_list->PushClipRect(header_bb.Min, header_bb.Max, true);
            draw_list->AddShadowCircle(
                ImVec2(title_pos.x + title_size.x * 0.5f, title_pos.y + title_size.y * 0.5f),
                Render::g_styling.apply_scale(16.0f),
                col(ImVec4(accent().x, accent().y, accent().z, 0.65f * accent_alpha())),
                Render::g_styling.apply_scale(64.0f),
                ImVec2(0.0f, 0.0f),
                0,
                24
            );
            draw_list->PopClipRect();

            draw_list->AddText(title_pos, col(Colors::SectionTitle()), title);

            if (collapse_anim > 0.01f)
            {
                const float line_y = header_bb.Max.y - 1.0f;
                const float line_alpha = 0.40f * collapse_anim * accent_alpha();
                draw_list->AddLine(
                    ImVec2(header_bb.Min.x + Layout::kSectionPadH(), line_y),
                    ImVec2(header_bb.Max.x - Layout::kSectionPadH(), line_y),
                    col(ImVec4(accent().x, accent().y, accent().z, line_alpha)),
                    1.0f
                );
            }
        }

        const bool render_content = collapse_anim > 0.01f;
        if (render_content)
        {
            const float stored_content_h = g_sectionContentHeight[section_id];
            const float content_top = start_pos.y + header_h + Layout::kSectionTitleGap();
            const float clip_bottom = content_top + stored_content_h * collapse_anim + Layout::kSectionPadV();

            if (stored_content_h > 0.0f)
            {
                draw_list->PushClipRect(
                    ImVec2(start_pos.x, start_pos.y + header_h),
                    ImVec2(start_pos.x + card_w, clip_bottom),
                    true
                );
                ImGui::GetStateStorage()->SetInt(ImGui::GetID("##sec_clip"), 1);
            }
            else
            {
                ImGui::GetStateStorage()->SetInt(ImGui::GetID("##sec_clip"), 0);
            }

            ImGui::SetCursorScreenPos(ImVec2(start_pos.x, content_top));
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
            ImGui::Indent(Layout::kSectionPadH());
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, collapse_anim * ImGui::GetStyle().Alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Render::g_styling.apply_scale(8.0f), Layout::kItemSpacingY()));
            ImGui::GetStateStorage()->SetFloat(ImGui::GetID("##sec_content_start_y"), content_top);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("##sec_content"), 1);
            return true;
        }

        ImGui::SetCursorScreenPos(ImVec2(start_pos.x, start_pos.y + header_h));
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::GetStateStorage()->SetInt(ImGui::GetID("##sec_content"), 0);
        ImGui::GetStateStorage()->SetInt(ImGui::GetID("##sec_clip"), 0);
        return false;
    }

    inline static void EndSection()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = window->DrawList;

        const ImGuiID section_id = ImGui::GetID("##section");
        const bool has_content = ImGui::GetStateStorage()->GetInt(ImGui::GetID("##sec_content")) != 0;
        const bool has_clip = ImGui::GetStateStorage()->GetInt(ImGui::GetID("##sec_clip")) != 0;
        const float collapse_anim = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("##sec_collapse_anim"));

        if (has_content)
        {
            const float content_start_y = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("##sec_content_start_y"));
            const float measured_content_h = ImMax(0.0f, ImGui::GetCursorScreenPos().y - content_start_y);
            g_sectionContentHeight[section_id] = measured_content_h;

            ImGui::PopStyleVar(2);
            ImGui::Unindent(Layout::kSectionPadH());

            if (has_clip)
                draw_list->PopClipRect();
        }

        const float start_y = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("##sec_start_y"));
        const float start_x = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("##sec_start_x"));
        const float header_h = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("##sec_header_h"));

        const ImVec2 start_pos(start_x, start_y);
        ImVec2 end_pos = ImGui::GetCursorScreenPos();
        end_pos.y += has_content ? Layout::kSectionPadV() : Render::g_styling.apply_scale(2.0f);
        end_pos.x = start_pos.x + Layout::SectionWidth();

        const float full_height = end_pos.y - start_pos.y;
        const float collapsed_height = header_h + 2.0f;
        const float animated_height = ImLerp(collapsed_height, full_height, collapse_anim);
        end_pos.y = start_pos.y + animated_height;

        draw_list->ChannelsSetCurrent(0);
        DrawSectionCard(draw_list, start_pos, end_pos, Layout::kSectionRounding());
        draw_list->ChannelsMerge();

        ImGui::SetCursorScreenPos(ImVec2(start_pos.x, end_pos.y + Layout::kSectionGap()));
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::PopID();
    }

    inline static bool VerticalTabs(std::string_view tab_name, std::string_view tab_ver, const char* id, const char** labels, int count, int* selected)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        const ImGuiStyle& style = ImGui::GetStyle();
        bool value_changed = false;

        const ImGuiID base_id = ImGui::GetID(id);
        const float sidebar_width = window->Size.x - style.WindowPadding.x;
        const float brand_pad_x = Render::g_styling.apply_scale(8.0f);
        const ImVec4 acc = accent();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        // Brand header
        {
            const float brand_h = Render::g_styling.apply_scale(52.0f);
            ImVec2 brand_pos = window->DC.CursorPos;
            ImRect brand_bb(brand_pos, brand_pos + ImVec2(sidebar_width, brand_h));

            ImGui::ItemSize(brand_bb);
            if (ImGui::ItemAdd(brand_bb, base_id + 9000))
            {
                ImVec2 glow_center(
                    brand_bb.Min.x + brand_pad_x + Render::g_styling.apply_scale(14.0f),
                    brand_bb.Min.y + brand_h * 0.5f
                );

                window->DrawList->AddShadowCircle(
                    glow_center,
                    Render::g_styling.apply_scale(5.0f),
                    col(ImVec4(acc.x, acc.y, acc.z, 0.55f)),
                    Render::g_styling.apply_scale(28.0f),
                    ImVec2(0.0f, 0.0f),
                    0,
                    24
                );

                window->DrawList->AddCircleFilled(
                    glow_center,
                    Render::g_styling.apply_scale(5.0f),
                    col(mul(acc, 1.0f)),
                    24
                );

                window->DrawList->AddText(
                    ImVec2(brand_bb.Min.x + brand_pad_x + Render::g_styling.apply_scale(30.0f), brand_bb.Min.y + Render::g_styling.apply_scale(12.0f)),
                    col(Colors::SectionTitle()),
                    tab_name.data()
                );

                window->DrawList->AddText(
                    ImVec2(brand_bb.Min.x + brand_pad_x + Render::g_styling.apply_scale(30.0f), brand_bb.Min.y + Render::g_styling.apply_scale(30.0f)),
                    col(ImVec4(0.55f, 0.55f, 0.60f, 1.0f)),
                    tab_ver.data()
                );
            }
        }

        const ImVec2 nav_block_start = window->DC.CursorPos;
        const float tab_height = Render::g_styling.apply_scale(36.0f);

        window->DrawList->AddRectFilled(
            nav_block_start,
            nav_block_start + ImVec2(sidebar_width, count * tab_height),
            col(accent_bg(0.10f, 0.30f)),
            0.0f
        );

        for (int i = 0; i < count; i++)
        {
            ImGuiID tab_id = base_id + i;
            ImGui::PushID(i);

            const bool is_selected = (*selected == i);

            auto it = s_tabAnim.find(tab_id);
            if (it == s_tabAnim.end())
                it = s_tabAnim.emplace(tab_id, is_selected ? 1.0f : 0.0f).first;

            float& anim_t = it->second;
            anim_t = AnimateToward(tab_id, is_selected ? 1.0f : 0.0f, 12.0f, s_tabAnim);

            const ImVec2 label_size = ImGui::CalcTextSize(labels[i]);
            const float pad_x = Render::g_styling.apply_scale(14.0f);

            ImVec2 pos = window->DC.CursorPos;
            ImRect bb(pos, pos + ImVec2(sidebar_width, tab_height));

            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, tab_id))
            {
                ImGui::PopID();
                continue;
            }

            bool hovered = false;
            bool held = false;
            const bool pressed = ImGui::ButtonBehavior(bb, tab_id, &hovered, &held);
            if (pressed)
            {
                *selected = i;
                value_changed = true;
            }

            ImVec4 bg_inactive = accent_bg(hovered ? 0.18f : 0.12f, hovered ? 0.38f : 0.28f);
            ImVec4 bg_active = accent_bg(0.22f, 0.52f);
            ImVec4 bg_color = ImLerp(bg_inactive, bg_active, anim_t);

            window->DrawList->AddRectFilled(bb.Min, bb.Max, col(bg_color), Render::g_styling.apply_scale(3.0f));

            DrawAccentGradientLine(
                window->DrawList,
                ImVec2(bb.Min.x, bb.Min.y),
                ImVec2(bb.Max.x, bb.Min.y + Render::g_styling.apply_scale(2.0f)),
                ImLerp(0.55f, 1.0f, anim_t)
            );

            
            if (anim_t > 0.01f)
            {
                const ImVec2 tab_center(bb.Min.x, (bb.Min.y + bb.Max.y) * 0.5f);
                window->DrawList->PushClipRect(bb.Min, bb.Max, true);
                window->DrawList->AddShadowCircle(
                    tab_center,
                    Render::g_styling.apply_scale(10.0f),
                    col(ImVec4(acc.x, acc.y, acc.z, 0.55f * anim_t * accent_alpha())),
                    Render::g_styling.apply_scale(48.0f),
                    ImVec2(0.0f, 0.0f),
                    0,
                    24
                );
                window->DrawList->PopClipRect();
            }

            ImVec4 text_inactive = ImVec4(0.66f, 0.66f, 0.71f, 1.00f);
            ImVec4 text_active = Colors::SectionTitle();
            ImVec4 text_color = ImLerp(text_inactive, text_active, anim_t);

            window->DrawList->AddText(
                ImVec2(bb.Min.x + pad_x + Render::g_styling.apply_scale(6.0f), bb.Min.y + (tab_height - label_size.y) * 0.5f),
                col(text_color),
                labels[i]
            );

            window->DC.CursorPos.y += tab_height / 6.f;
            ImGui::PopID();
        }

        ImGui::PopStyleVar();
        return value_changed;
    }

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count, bool disabled = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;

        const bool is_disabled = IsWidgetDisabled(disabled);
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        const float width = Layout::ContentWidth();
        const float control_h = Layout::kControlHeight();
        const float row_h = label_size.y + Layout::kLabelControlGap() + control_h;

        ImVec2 row_pos = window->DC.CursorPos;
        ImRect total_bb(row_pos, row_pos + ImVec2(width, row_h));

        ImVec2 frame_pos(row_pos.x, row_pos.y + label_size.y + Layout::kLabelControlGap());
        ImRect frame_bb(frame_pos, frame_pos + ImVec2(width, control_h));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
            return false;

        DrawLabel(row_pos, label, is_disabled);

        ImGui::PushID(label);
        const char* popup_name = "##combo_popup";

        bool hovered = false;
        bool pressed = false;
        if (!is_disabled)
        {
            hovered = ImGui::ItemHoverable(frame_bb, id, 0);
            pressed = hovered && io.MouseClicked[0];
            if (pressed)
                ImGui::OpenPopup(popup_name);
        }

        float open_t = AnimateToward(id, ImGui::IsPopupOpen(popup_name) ? 1.0f : 0.0f, 14.0f, g_comboAnim);

        const char* preview = (*current_item >= 0 && *current_item < items_count)
            ? items[*current_item]
            : "Select";

        ImVec4 bg = mul(accent(), is_disabled ? 0.12f : (hovered ? 0.26f : 0.20f));
        window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, col(bg), Render::g_styling.apply_scale(4.0f));

        ImVec2 preview_size = ImGui::CalcTextSize(preview);
        window->DrawList->AddText(
            ImVec2(frame_bb.Min.x + Render::g_styling.apply_scale(10.0f), frame_bb.Min.y + (control_h - preview_size.y) * 0.5f),
            WidgetLabelCol(is_disabled),
            preview
        );

        const float arrow_size = Render::g_styling.apply_scale(5.0f);
        ImVec2 arrow_center(
            frame_bb.Max.x - Render::g_styling.apply_scale(14.0f),
            frame_bb.Min.y + control_h * 0.5f
        );
        ImU32 arrow_col = WidgetLabelCol(is_disabled);
        window->DrawList->AddTriangleFilled(
            ImVec2(arrow_center.x - arrow_size * 0.6f, arrow_center.y - arrow_size * 0.35f + open_t * Render::g_styling.apply_scale(2.0f)),
            ImVec2(arrow_center.x + arrow_size * 0.6f, arrow_center.y - arrow_size * 0.35f + open_t * Render::g_styling.apply_scale(2.0f)),
            ImVec2(arrow_center.x, arrow_center.y + arrow_size * 0.65f - open_t * Render::g_styling.apply_scale(2.0f)),
            arrow_col
        );

        bool value_changed = false;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Render::g_styling.apply_scale(6.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Render::g_styling.apply_scale(4.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, Render::g_styling.apply_scale(4.0f));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, mul(accent(), 0.14f));
        ImGui::PushStyleColor(ImGuiCol_Header, mul(accent(), 0.28f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, mul(accent(), 0.40f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, mul(accent(), 0.50f));

        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0.0f), ImVec2(width, FLT_MAX));
        if (ImGui::BeginPopup(popup_name))
        {
            for (int i = 0; i < items_count; ++i)
            {
                ImGui::PushID(i);
                const bool selected = (*current_item == i);
                if (ImGui::Selectable(items[i], selected, 0, ImVec2(width - Render::g_styling.apply_scale(8.0f), control_h)))
                {
                    *current_item = i;
                    value_changed = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(3);

        window->DC.CursorPos = ImVec2(row_pos.x, row_pos.y + row_h + Layout::kWidgetGap());
        return value_changed;
    }

    inline static bool Checkbox(const char* label, bool* v, bool disabled = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;

        const bool is_disabled = IsWidgetDisabled(disabled);
        const ImGuiID id = window->GetID(label);
        const float width = Layout::ContentWidth();
        const float row_h = Layout::kRowHeight();
        const float toggle_h = Layout::kControlHeight();
        const float toggle_w = Layout::kToggleWidth();
        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

        ImVec2 row_pos = window->DC.CursorPos;
        ImRect total_bb(row_pos, row_pos + ImVec2(width, row_h));

        ImVec2 toggle_pos(
            row_pos.x + width - toggle_w,
            row_pos.y + (row_h - toggle_h) * 0.5f
        );
        ImRect frame_bb(toggle_pos, toggle_pos + ImVec2(toggle_w, toggle_h));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
            return false;

        bool hovered = false;
        bool pressed = false;
        if (!is_disabled)
        {
            bool held;
            pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
            if (pressed)
                *v = !*v;
        }

        float t = AnimateToward(id, *v ? 1.0f : 0.0f, 12.0f, g_checkboxAnim);
        if (is_disabled)
            t *= 0.45f;

        const float pad_y = toggle_h * 0.18f;
        const float pad_x = toggle_h * 0.15f;
        const float knob_r = (toggle_h - 2.0f * pad_y) * 0.5f;

        const float track_left = frame_bb.Min.x + pad_x;
        const float track_right = frame_bb.Max.x - pad_x;
        const float track_top = frame_bb.Min.y + pad_y;
        const float track_bot = frame_bb.Max.y - pad_y;

        const float knob_off_x = track_left + knob_r;
        const float knob_on_x = track_right - knob_r;
        const float knob_y = (track_top + track_bot) * 0.5f;
        float center_x = ImLerp(knob_off_x, knob_on_x, t);

        ImVec4 bg_inactive = mul(accent(), is_disabled ? 0.12f : (hovered ? 0.24f : 0.18f));
        ImVec4 bg_active = mul(accent(), is_disabled ? 0.22f : 0.42f);
        ImVec4 bg_color = ImLerp(bg_inactive, bg_active, t);

        window->DrawList->AddRectFilled(
            ImVec2(track_left, track_top),
            ImVec2(track_right, track_bot),
            ImGui::ColorConvertFloat4ToU32(bg_color),
            toggle_h * 0.5f
        );

        window->DrawList->AddCircleFilled(
            ImVec2(center_x, knob_y),
            knob_r,
            col(mul(accent(), is_disabled ? 0.45f : 1.0f)),
            32
        );

        if (label_size.x > 0.0f)
        {
            DrawLabel(
                ImVec2(row_pos.x, row_pos.y + (row_h - label_size.y) * 0.5f),
                label,
                is_disabled
            );
        }

        window->DC.CursorPos = ImVec2(row_pos.x, row_pos.y + row_h + Layout::kWidgetGap());
        return pressed;
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f", bool disabled = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;

        const bool is_disabled = IsWidgetDisabled(disabled);
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        const float width = Layout::ContentWidth();
        const float slider_height = Layout::kControlHeight();
        const float row_h = label_size.y + Layout::kSliderLabelGap() + slider_height;

        ImVec2 row_pos = window->DC.CursorPos;
        ImRect total_bb(row_pos, row_pos + ImVec2(width, row_h));

        ImVec2 frame_pos(row_pos.x, row_pos.y + label_size.y + Layout::kSliderLabelGap());
        ImRect frame_bb(frame_pos, frame_pos + ImVec2(width, slider_height));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
            return false;

        bool hovered = false;
        bool value_changed = false;

        if (!is_disabled)
        {
            hovered = ImGui::ItemHoverable(frame_bb, id, 0);

            if (hovered && io.MouseClicked[0])
            {
                ImGui::SetActiveID(id, window);
                ImGui::SetFocusID(id, window);
                ImGui::FocusWindow(window);
            }

            if (g.ActiveId == id)
            {
                if (io.MouseDown[0])
                {
                    float mouse_abs_pos = io.MousePos.x;
                    float clicked_t = ImSaturate((mouse_abs_pos - frame_bb.Min.x) / (frame_bb.Max.x - frame_bb.Min.x));
                    float new_value = ImLerp(v_min, v_max, clicked_t);

                    if (*v != new_value)
                    {
                        *v = new_value;
                        value_changed = true;
                    }
                }
                else
                {
                    ImGui::ClearActiveID();
                }
            }
        }

        float target_t = ImSaturate((*v - v_min) / (v_max - v_min));
        float t = AnimateToward(id, target_t, 16.0f, g_sliderAnim);

        float track_height = Render::g_styling.apply_scale(6.0f);
        float track_y_center = (frame_bb.Min.y + frame_bb.Max.y) * 0.5f;
        ImVec2 track_min(frame_bb.Min.x, track_y_center - track_height * 0.5f);
        ImVec2 track_max(frame_bb.Max.x, track_y_center + track_height * 0.5f);

        float track_alpha = is_disabled ? 0.12f : (hovered ? 0.24f : 0.18f);
        window->DrawList->AddRectFilled(
            track_min, track_max,
            col(mul(accent(), track_alpha)),
            track_height * 0.5f);

        float handle_x = ImLerp(frame_bb.Min.x, frame_bb.Max.x, t);

        window->DrawList->AddRectFilled(
            track_min,
            ImVec2(handle_x, track_max.y),
            col(mul(accent(), is_disabled ? 0.35f : 0.85f)),
            track_height * 0.5f);

        float handle_radius = Render::g_styling.apply_scale(5.0f);
        float handle_scale = (!is_disabled && (hovered || g.ActiveId == id)) ? 1.15f : 1.0f;
        window->DrawList->AddCircleFilled(
            ImVec2(handle_x, track_y_center),
            handle_radius * handle_scale,
            col(mul(accent(), is_disabled ? 0.40f : 1.2f)),
            32);

        DrawLabel(row_pos, label, is_disabled);

        char value_buf[64];
        snprintf(value_buf, sizeof(value_buf), format, *v);
        ImVec2 val_size = ImGui::CalcTextSize(value_buf);
        window->DrawList->AddText(
            ImVec2(row_pos.x + width - val_size.x, row_pos.y),
            ValueTextCol(is_disabled),
            value_buf
        );

        window->DC.CursorPos = ImVec2(row_pos.x, row_pos.y + row_h + Layout::kWidgetGap());
        return value_changed;
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max, bool disabled = false)
    {
        float vf = (float)*v;
        bool changed = SliderFloat(label, &vf, (float)v_min, (float)v_max, "%.0f", disabled);
        int new_v = (int)(vf + 0.5f);
        if (changed || new_v != *v)
        {
            *v = new_v;
            return true;
        }
        return false;
    }

    bool ColorEdit4(const char* label, ImColor* color, ImGuiColorEditFlags extra_flags = 0, bool disabled = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        const bool is_disabled = IsWidgetDisabled(disabled);
        const ImGuiID id = window->GetID(label);
        const float width = Layout::ContentWidth();
        const float row_h = Layout::kRowHeight();
        const float btn_size = Layout::kControlHeight();
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        ImVec2 row_pos = window->DC.CursorPos;
        ImRect total_bb(row_pos, row_pos + ImVec2(width, row_h));

        ImVec2 btn_pos(
            row_pos.x + width - btn_size,
            row_pos.y + (row_h - btn_size) * 0.5f
        );
        ImRect btn_bb(btn_pos, btn_pos + ImVec2(btn_size, btn_size));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &btn_bb))
            return false;

        DrawLabel(
            ImVec2(row_pos.x, row_pos.y + (row_h - label_size.y) * 0.5f),
            label,
            is_disabled
        );

        ImGuiColorEditFlags flags =
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoSidePreview |
            ImGuiColorEditFlags_AlphaBar |
            extra_flags;

        ImGui::PushID(label);
        window->DC.CursorPos = btn_pos;

        if (is_disabled)
            ImGui::BeginDisabled();

        bool changed = false;
        if (ImGui::ColorButton("##color_btn", color->Value, flags, ImVec2(btn_size, btn_size)))
            ImGui::OpenPopup("##color_popup");

        if (ImGui::BeginPopup("##color_popup"))
        {
            changed = ImGui::ColorPicker4(
                "##color_picker",
                (float*)&color->Value,
                flags | ImGuiColorEditFlags_NoLabel
            );
            ImGui::EndPopup();
        }

        if (is_disabled)
            ImGui::EndDisabled();

        ImGui::PopID();

        window->DC.CursorPos = ImVec2(row_pos.x, row_pos.y + row_h + Layout::kWidgetGap());
        return changed;
    }

    bool Keybind(const char* label, int* key, bool disabled = false)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;

        const bool is_disabled = IsWidgetDisabled(disabled);
        const ImGuiID id = window->GetID(label);
        const float width = Layout::ContentWidth();
        const float row_h = Layout::kRowHeight();
        const float btn_w = ImMax(Render::g_styling.apply_scale(72.0f), width * 0.38f);
        const float btn_h = Layout::kControlHeight();
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        ImVec2 row_pos = window->DC.CursorPos;
        ImRect total_bb(row_pos, row_pos + ImVec2(width, row_h));

        ImVec2 btn_pos(
            row_pos.x + width - btn_w,
            row_pos.y + (row_h - btn_h) * 0.5f
        );
        ImRect btn_bb(btn_pos, btn_pos + ImVec2(btn_w, btn_h));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &btn_bb))
            return false;

        bool& listening = g_keybindListening[id];
        bool hovered = false;
        bool pressed = false;

        if (!is_disabled)
        {
            bool held;
            pressed = ImGui::ButtonBehavior(btn_bb, id, &hovered, &held);
            if (pressed)
                listening = !listening;
        }
        else
        {
            listening = false;
        }

        bool value_changed = false;
        if (listening && !is_disabled)
        {
            for (int vk = 1; vk < 256; ++vk)
            {
                if (vk == VK_ESCAPE)
                    continue;

                if (GetAsyncKeyState(vk) & 0x8000)
                {
                    if (vk == VK_LBUTTON || vk == VK_RBUTTON)
                        continue;

                    *key = vk;
                    listening = false;
                    value_changed = true;
                    break;
                }
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                listening = false;
        }

        float pulse = listening ? (0.5f + 0.5f * sinf((float)ImGui::GetTime() * 6.0f)) : 0.0f;
        ImVec4 bg = mul(accent(), is_disabled ? 0.12f : (listening ? 0.30f + pulse * 0.15f : (hovered ? 0.26f : 0.20f)));

        window->DrawList->AddRectFilled(
            btn_bb.Min,
            btn_bb.Max,
            col(bg),
            Render::g_styling.apply_scale(4.0f)
        );

        const char* key_text = listening ? "..." : (*key ? VkToString(*key) : "None");
        ImVec2 key_size = ImGui::CalcTextSize(key_text);
        window->DrawList->AddText(
            ImVec2(
                btn_bb.Min.x + (btn_w - key_size.x) * 0.5f,
                btn_bb.Min.y + (btn_h - key_size.y) * 0.5f
            ),
            WidgetLabelCol(is_disabled),
            key_text
        );

        DrawLabel(
            ImVec2(row_pos.x, row_pos.y + (row_h - label_size.y) * 0.5f),
            label,
            is_disabled
        );

        window->DC.CursorPos = ImVec2(row_pos.x, row_pos.y + row_h + Layout::kWidgetGap());
        return value_changed;
    }
}
