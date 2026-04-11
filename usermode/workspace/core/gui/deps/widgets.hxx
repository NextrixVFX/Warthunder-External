#pragma once

using namespace ImGui;

inline static std::unordered_map<ImGuiID, float> g_checkboxAnim;
inline static std::unordered_map<ImGuiID, float> s_tabAnim;

namespace External::UI::CustomWidgets
{
    // Section with rounded background box
    inline static void BeginSection(const char* title, const ImVec4& bg_color = ImVec4(0.11f, 0.11f, 0.16f, 1.0f))
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = window->DrawList;

        // Split channels: 0 = background, 1 = foreground
        draw_list->ChannelsSplit(2);
        draw_list->ChannelsSetCurrent(1); // Draw content on foreground

        ImGui::Dummy(ImVec2(0, 0));

        // Store cursor position for background rect
        ImVec2 start_pos = ImGui::GetCursorScreenPos();

        ImGui::Dummy(ImVec2(0, 0));

        // Title with left padding
        ImGui::Indent(12.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
        ImGui::Text("%s", title);
        ImGui::PopStyleColor();
        ImGui::Spacing();

        // Store section start and track that we're indented
        ImGui::GetStateStorage()->SetFloat(ImGui::GetID("section_start_y"), start_pos.y);
        ImGui::GetStateStorage()->SetFloat(ImGui::GetID("section_start_x"), start_pos.x);
    }

    inline static void EndSection()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = window->DrawList;

        // Unindent before getting end position
        ImGui::Unindent(12.0f);

        // Get stored section start
        float start_y = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("section_start_y"));
        float start_x = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("section_start_x"));

        ImVec2 start_pos(start_x, start_y);
        ImVec2 end_pos = ImGui::GetCursorScreenPos();
        end_pos.y += 6.0f; // bottom padding

        // Properly calculate width relative to column's actual width
        float column_width = ImGui::GetColumnWidth();
        end_pos.x = start_pos.x + column_width - 18.0f;

        // Switch to background channel and draw rect
        draw_list->ChannelsSetCurrent(0);

        draw_list->AddRectFilled(
            start_pos,
            end_pos,
            IM_COL32(28, 28, 38, 255),
            6.0f
        );

        // Merge channels
        draw_list->ChannelsMerge();

        ImGui::Dummy(ImVec2(0, 0));
    }

    inline static bool VerticalTabs(const char* id, const char** labels, int count, int* selected)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;
        bool value_changed = false;

        const ImGuiID base_id = ImGui::GetID(id);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0.0f));

        ImVec2 orgin = window->DC.CursorPos;

        for (int i = 0; i < count; i++)
        {
            ImGuiID tab_id = base_id + i;
            ImGui::PushID(i);

            bool is_selected = (*selected == i);

            // Get or create animation value for this tab
            auto it = s_tabAnim.find(tab_id);
            if (it == s_tabAnim.end())
                it = s_tabAnim.emplace(tab_id, is_selected ? 1.0f : 0.0f).first;

            float& anim_t = it->second;

            // Animate toward target (0 = inactive, 1 = active)
            float target = is_selected ? 1.0f : 0.0f;
            const float speed = 10.0f;
            anim_t += (target - anim_t) * ImClamp(speed * io.DeltaTime, 0.0f, 1.0f);
            if (fabsf(target - anim_t) < 0.001f)
                anim_t = target;

            // Layout
            const ImVec2 label_size = ImGui::CalcTextSize(labels[i]);
            const float tab_height = 40.0f;
            const float pad_x = 16.0f;

            ImVec2 pos = window->DC.CursorPos;
            ImRect bb(pos, pos + ImVec2(window->Size.x - style.WindowPadding.x * 2, tab_height));

            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, tab_id))
            {
                ImGui::PopID();
                continue;
            }

            // Interaction
            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb, tab_id, &hovered, &held);
            if (pressed)
            {
                *selected = i;
                value_changed = true;
            }

            // Animated colors
            ImVec4 bg_inactive = ImVec4(0.09f, 0.09f, 0.13f, 1.00f);
            ImVec4 bg_active = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
            ImVec4 bg_hover = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);

            ImVec4 text_inactive = ImVec4(0.65f, 0.65f, 0.70f, 1.00f);
            ImVec4 text_active = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);

            // Lerp background color
            ImVec4 bg_color = ImLerp(bg_inactive, bg_active, anim_t);

            // Lerp text color
            ImVec4 text_color = ImLerp(text_inactive, text_active, anim_t);

            // Draw rounded rect background
            window->DrawList->AddRectFilled(
                bb.Min,
                bb.Max,
                ImGui::ColorConvertFloat4ToU32(bg_color),
                6.0f,
                ImDrawFlags_RoundCornersRight
            );

            // Optional: left accent bar when active
            float accent_width = ImLerp(0.0f, 4.0f, anim_t);
            float accent_height = 0.0f;// ImLerp(4.0f, 0.0f, anim_t);
            ImVec2 accent_min(bb.Min.x, bb.Min.y + accent_height);
            ImVec2 accent_max(bb.Min.x + accent_width, bb.Max.y - accent_height);

            ImU32 accent_col = ImGui::ColorConvertFloat4ToU32(
                ImVec4(0.40f, 0.65f, 1.00f, anim_t)
            );

            window->DrawList->AddRectFilled(
                accent_min,
                accent_max,
                accent_col
            );

            // Draw text centered vertically
            ImVec2 text_pos(
                bb.Min.x + pad_x + ImLerp(0.0, 8.0, anim_t),  // indent when active
                bb.Min.y + (tab_height - label_size.y) * 0.5f
            );

            window->DrawList->AddText(
                text_pos,
                ImGui::ColorConvertFloat4ToU32(text_color),
                labels[i]
            );

            ImGui::PopID();
        }

        ImVec2 time_pos(
            orgin.x + 6.0f, orgin.y + window->Size.y - 36.0f
        );

        window->DrawList->AddText(
            time_pos,
            ImGui::ColorConvertFloat4ToU32({ 0.8f, 0.8f, 0.9f, 0.7f }),
            std::format("Time Remaining:\t{}", "LIFE").c_str()
        );

        ImGui::PopStyleVar();
        return value_changed;
    }

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        float combo_width = 140.0f;  // Match slider width
        float combo_height = 24.0f;

        // Label above combo
        ImVec2 label_pos = window->DC.CursorPos;
        ImGui::RenderText(label_pos, label);
        window->DC.CursorPos.y += label_size.y + 4.0f;

        // Combo frame
        ImVec2 combo_pos = window->DC.CursorPos;

        ImGui::PushItemWidth(combo_width);
        bool value_changed = ImGui::Combo(("##" + std::string(label)).c_str(), current_item, items, items_count);
        ImGui::PopItemWidth();

        // Reduce spacing
        window->DC.CursorPos.y -= 2.0f;

        return value_changed;
    }

    inline static bool Checkbox(const char* label, bool* v)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const float h = ImGui::GetFrameHeight();
        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

        ImRect frame_bb(window->DC.CursorPos,
            window->DC.CursorPos + ImVec2(h * 2.0f, h));
        ImRect total_bb(frame_bb.Min,
            frame_bb.Max + ImVec2(style.ItemInnerSpacing.x + label_size.x, 0.0f));

        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
        if (pressed)
            *v = !*v;

        // Get or create animation value for THIS checkbox
        auto it = g_checkboxAnim.find(id);
        if (it == g_checkboxAnim.end())
        {
            // First time: initialize to current state (no animation)
            it = g_checkboxAnim.emplace(id, *v ? 1.0f : 0.0f).first;
        }
        float& t = it->second;

        // Animate toward target
        float target = *v ? 1.0f : 0.0f;
        const float speed = 12.0f;
        t += (target - t) * ImClamp(speed * io.DeltaTime, 0.0f, 1.0f);
        if (fabsf(target - t) < 0.001f)
            t = target;

        // Geometry
        const float pad_y = h * 0.15f;
        const float pad_x = h * 0.15f;
        const float knob_r = (h - 2.0f * pad_y) * 0.5f;

        const float track_left = frame_bb.Min.x + pad_x;
        const float track_right = frame_bb.Max.x - pad_x;
        const float track_top = frame_bb.Min.y + pad_y;
        const float track_bot = frame_bb.Max.y - pad_y;

        // Knob center positions
        const float knob_off_x = track_left + knob_r;
        const float knob_on_x = track_right - knob_r;
        const float knob_y = (track_top + track_bot) * 0.5f;

        float center_x = ImLerp(knob_off_x, knob_on_x, t);

        // Track
        // Animated colors
        ImVec4 bg_inactive = ImVec4(0.13f, 0.13f, 0.18f, 1.00f);
        ImVec4 bg_active = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);

        // Lerp background color
        ImVec4 bg_color = ImLerp(bg_inactive, bg_active, t);

        window->DrawList->AddRectFilled(
            ImVec2(track_left, track_top),
            ImVec2(track_right, track_bot),
            ImGui::ColorConvertFloat4ToU32(bg_color),
            h * 0.5f
        );

        // Knob
        window->DrawList->AddCircleFilled(
            ImVec2(center_x, knob_y),
            knob_r,
            ImGui::GetColorU32(ImGuiCol_CheckMark),
            32
        );

        // Label
        if (label_size.x > 0.0f)
            ImGui::RenderText(
                ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x,
                    frame_bb.Min.y + style.FramePadding.y),
                label
            );

        return pressed;
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        float width = 130.0f;
        float slider_height = 20.0f;

        ImVec2 label_pos = window->DC.CursorPos;
        window->DC.CursorPos.y += label_size.y + 4.0f;

        ImRect frame_bb(window->DC.CursorPos,
            window->DC.CursorPos + ImVec2(width, slider_height));

        ImRect total_bb(label_pos,
            ImVec2(frame_bb.Max.x, frame_bb.Max.y));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
            return false;

        bool hovered = ImGui::ItemHoverable(frame_bb, id, 0);
        bool value_changed = false;

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

        // Visual rendering
        float track_height = 6.0f;
        float track_y_center = (frame_bb.Min.y + frame_bb.Max.y) * 0.5f;
        ImVec2 track_min(frame_bb.Min.x, track_y_center - track_height * 0.5f);
        ImVec2 track_max(frame_bb.Max.x, track_y_center + track_height * 0.5f);

        window->DrawList->AddRectFilled(
            track_min, track_max,
            IM_COL32(40, 40, 50, 255),
            track_height * 0.5f);

        float t = ImSaturate((*v - v_min) / (v_max - v_min));
        float handle_x = ImLerp(frame_bb.Min.x, frame_bb.Max.x, t);

        window->DrawList->AddRectFilled(
            track_min,
            ImVec2(handle_x, track_max.y),
            IM_COL32(80, 140, 255, 255),
            track_height * 0.5f);

        float handle_radius = 5.0f;
        window->DrawList->AddCircleFilled(
            ImVec2(handle_x, track_y_center),
            handle_radius,
            IM_COL32(230, 230, 240, 255),
            32);

        ImGui::RenderText(label_pos, label);

        char value_buf[64];
        snprintf(value_buf, sizeof(value_buf), "%.1f", *v);
        ImVec2 val_size = ImGui::CalcTextSize(value_buf);
        ImGui::RenderText(
            ImVec2(frame_bb.Max.x - val_size.x, label_pos.y),
            value_buf
        );

        // Reduce spacing after this widget
        window->DC.CursorPos.y -= 12.0f;

        return value_changed;
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        float width = 130.0f;
        float slider_height = 20.0f;

        ImVec2 label_pos = window->DC.CursorPos;
        window->DC.CursorPos.y += label_size.y + 4.0f;

        ImRect frame_bb(window->DC.CursorPos,
            window->DC.CursorPos + ImVec2(width, slider_height));

        ImRect total_bb(label_pos,
            ImVec2(frame_bb.Max.x, frame_bb.Max.y));

        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
            return false;

        bool hovered = ImGui::ItemHoverable(frame_bb, id, 0);
        bool value_changed = false;

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
                float new_value_f = ImLerp((float)v_min, (float)v_max, clicked_t);
                int new_value = (int)(new_value_f + 0.5f);

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

        // Visual rendering
        float track_height = 6.0f;
        float track_y_center = (frame_bb.Min.y + frame_bb.Max.y) * 0.5f;
        ImVec2 track_min(frame_bb.Min.x, track_y_center - track_height * 0.5f);
        ImVec2 track_max(frame_bb.Max.x, track_y_center + track_height * 0.5f);

        window->DrawList->AddRectFilled(
            track_min, track_max,
            IM_COL32(40, 40, 50, 255),
            track_height * 0.5f);

        float t = ImSaturate(((float)*v - (float)v_min) / ((float)v_max - (float)v_min));
        float handle_x = ImLerp(frame_bb.Min.x, frame_bb.Max.x, t);

        window->DrawList->AddRectFilled(
            track_min,
            ImVec2(handle_x, track_max.y),
            IM_COL32(80, 140, 255, 255),
            track_height * 0.5f);

        float handle_radius = 5.0f;
        window->DrawList->AddCircleFilled(
            ImVec2(handle_x, track_y_center),
            handle_radius,
            IM_COL32(230, 230, 240, 255),
            32);

        ImGui::RenderText(label_pos, label);

        char value_buf[64];
        snprintf(value_buf, sizeof(value_buf), "%d", *v);
        ImVec2 val_size = ImGui::CalcTextSize(value_buf);
        ImGui::RenderText(
            ImVec2(frame_bb.Max.x - val_size.x, label_pos.y),
            value_buf
        );

        window->DC.CursorPos.y -= 12.0f;

        return value_changed;
    }


}

