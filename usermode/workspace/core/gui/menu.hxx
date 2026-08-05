#pragma once

namespace External::Render
{
	class c_menu
	{
    public:
        static inline bool menu_active = true;
        inline static float menu_alpha = 1.0f;

    private:
        inline static int s_lastCategory = 0;
        inline static float s_fadeAnim = 1.0f;
        inline static float s_pageSlideDir = 1.0f;
        inline static const char* _boxTypes[] = { "Regular", "Filled", "Corner", "Corner-Filled", "Three-Dimensional" };
        inline static bool menu_setup = false;

        static float ease_out_cubic(float t)
        {
            t = ImClamp(t, 0.0f, 1.0f);
            const float inv = 1.0f - t;
            return 1.0f - inv * inv * inv;
        }

        static void draw_active_panel(int category, float eased_alpha, float slide_x, float slide_y)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, eased_alpha * ImGui::GetStyle().Alpha);

            const ImVec2 panel_pos = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(ImVec2(panel_pos.x + slide_x, panel_pos.y + slide_y));
            ImGui::Dummy(ImVec2(0.0f, 0.0f));

            switch (category)
            {
                case 0: DrawVisualsPanel(); break;
                case 1: DrawSettingsPanel(); break;
                default: DrawVisualsPanel(); break;
            }

            ImGui::PopStyleVar();
        }

    public:
        static void update_visibility()
        {
            if (!g_window.m_window)
                return;

            LONG_PTR ex = GetWindowLongPtr(g_window.m_window, GWL_EXSTYLE);

            if (ImGui::GetIO().WantCaptureMouse)
            {
                ex &= ~WS_EX_TRANSPARENT;
            }
            else
            {
                ex |= WS_EX_TRANSPARENT;
            }

            SetWindowLongPtr(g_window.m_window, GWL_EXSTYLE, ex);
        }

        static void setup_menu()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            
            auto mul = [](ImVec4 c, float v)
            {
                return ImVec4(
                    c.x * v,
                    c.y * v,
                    c.z * v,
                    c.w
                );
            };

            const ImVec4 accent = g_styling.accent();

            style.WindowRounding = g_styling.menu_rounding();
            style.FrameRounding = g_styling.elements_rounding();
            style.GrabRounding = g_styling.elements_rounding();
            style.ScrollbarRounding = g_styling.elements_rounding();
            style.TabRounding = g_styling.elements_rounding();
            style.ChildRounding = g_styling.child_rounding();

            style.WindowPadding = g_styling.apply_scale(8.0f, 8.0f);
            style.FramePadding = g_styling.apply_scale(6.0f, 4.0f);
            style.ItemSpacing = g_styling.apply_scale(8.0f, 6.0f);
            style.ItemInnerSpacing = g_styling.apply_scale(8.0f, 4.0f);
            style.IndentSpacing = g_styling.apply_scale(20.0f);
            style.ScrollbarSize = g_styling.apply_scale(12.0f);
            style.GrabMinSize = g_styling.apply_scale(8.0f);

            style.WindowBorderSize = 1.0f;
            style.ChildBorderSize = 1.0f;
            style.FrameBorderSize = 0.0f;

            style.ColumnsMinSpacing = g_styling.apply_scale(12.0f);

            ImVec4* colors = style.Colors;

            // backgrounds
            colors[ImGuiCol_WindowBg] = mul(accent, 0.08f);
            colors[ImGuiCol_ChildBg] = mul(accent, 0.10f);
            colors[ImGuiCol_PopupBg] = mul(accent, 0.12f);

            // borders
            colors[ImGuiCol_Border] = mul(accent, 0.22f);

            // frames
            colors[ImGuiCol_FrameBg] = mul(accent, 0.18f);
            colors[ImGuiCol_FrameBgHovered] = mul(accent, 0.28f);
            colors[ImGuiCol_FrameBgActive] = mul(accent, 0.40f);

            // buttons
            colors[ImGuiCol_Button] = mul(accent, 0.22f);
            colors[ImGuiCol_ButtonHovered] = mul(accent, 0.38f);
            colors[ImGuiCol_ButtonActive] = mul(accent, 0.55f);

            // headers
            colors[ImGuiCol_Header] = mul(accent, 0.24f);
            colors[ImGuiCol_HeaderHovered] = mul(accent, 0.38f);
            colors[ImGuiCol_HeaderActive] = mul(accent, 0.50f);

            // tabs
            colors[ImGuiCol_Tab] = mul(accent, 0.16f);
            colors[ImGuiCol_TabHovered] = mul(accent, 0.34f);
            colors[ImGuiCol_TabActive] = mul(accent, 0.45f);

            // title bars
            colors[ImGuiCol_TitleBg] = mul(accent, 0.12f);
            colors[ImGuiCol_TitleBgActive] = mul(accent, 0.20f);

            // scrollbar
            colors[ImGuiCol_ScrollbarBg] = mul(accent, 0.10f);
            colors[ImGuiCol_ScrollbarGrab] = mul(accent, 0.26f);
            colors[ImGuiCol_ScrollbarGrabHovered] = mul(accent, 0.38f);
            colors[ImGuiCol_ScrollbarGrabActive] = mul(accent, 0.50f);

            // separators
            colors[ImGuiCol_Separator] = mul(accent, 0.24f);
            colors[ImGuiCol_SeparatorHovered] = mul(accent, 0.40f);
            colors[ImGuiCol_SeparatorActive] = mul(accent, 0.55f);

            // resize grip
            colors[ImGuiCol_ResizeGrip] = mul(accent, 0.30f);
            colors[ImGuiCol_ResizeGripHovered] = mul(accent, 0.45f);
            colors[ImGuiCol_ResizeGripActive] = mul(accent, 0.60f);

            // checkmarks/sliders
            colors[ImGuiCol_CheckMark] = mul(accent, 1.00f);
            colors[ImGuiCol_SliderGrab] = mul(accent, 0.80f);
            colors[ImGuiCol_SliderGrabActive] = mul(accent, 1.00f);

            // selected text
            colors[ImGuiCol_TextSelectedBg] = ImVec4(
                accent.x,
                accent.y,
                accent.z,
                0.35f
            );

            colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.60f, 1.00f);

            menu_setup = true;
        }

        static void render_menu()
        {
            setup_menu();

            static float s_last_menu_scale = 0.f;
            const float scale = g_styling.get_scale();
            const ImVec2 default_size = g_styling.menu_size();
            const ImVec2 min_size = g_styling.menu_min_size();

            ImGui::SetNextWindowSizeConstraints(min_size, ImVec2(FLT_MAX, FLT_MAX));
            if (std::fabs(scale - s_last_menu_scale) > 0.02f)
            {
                ImGui::SetNextWindowSize(default_size);
                s_last_menu_scale = scale;
            }
            else
            {
                ImGui::SetNextWindowSize(default_size, ImGuiCond_FirstUseEver);
            }

            ImGui::Begin(
                encrypt("##main_panel"),
                nullptr,
                ImGuiWindowFlags_NoTitleBar
            );

            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();

            ImDrawList* bg_draw_list = ImGui::GetBackgroundDrawList();

            int shadow_layers = 12;
            float shadow_offset = 0.0f;
            float shadow_spread = g_styling.apply_scale(10.0f);

            auto mul = [](ImVec4 c, float v, float a)
                {
                    return ImColor(
                        c.x * v,
                        c.y * v,
                        c.z * v,
                        a
                    );
                };

            const ImVec4 accent = g_styling.accent();

            for (int i = shadow_layers; i > 0; i--)
            {
                float layer_progress = (float)i / (float)shadow_layers;
                float thickness = shadow_spread * layer_progress;
                float alpha = (1.0f - layer_progress) * 0.4f;

                ImVec2 shadow_min = ImVec2(
                    window_pos.x - thickness + shadow_offset,
                    window_pos.y - thickness + shadow_offset
                );
                ImVec2 shadow_max = ImVec2(
                    window_pos.x + window_size.x + thickness + shadow_offset,
                    window_pos.y + window_size.y + thickness + shadow_offset
                );

                bg_draw_list->AddRectFilled(
                    shadow_min,
                    shadow_max,
                    mul(accent, 0.10f, alpha),
                    g_styling.menu_rounding() + thickness * 0.3f
                );
            }

            const float content_width =
                ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
            const float sidebar_width = ImClamp(
                content_width * 0.24f,
                g_styling.apply_scale(130.0f),
                g_styling.apply_scale(190.0f)
            );

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
            ImGui::BeginChild("##left", ImVec2(sidebar_width, 0), false);

            static int category = 0;
            const char* tabs[] = { "Visuals", "Settings" };
            std::string tab_name = "novision.cc";
            std::string tab_ver = "v26.0.0.0";
            Widgets::VerticalTabs(tab_name, tab_ver, "##sidebar", tabs, 2, &category);

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            const ImVec2 sidebar_min = ImGui::GetItemRectMin();
            const ImVec2 sidebar_max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(sidebar_max.x + g_styling.apply_scale(4.0f), sidebar_min.y + g_styling.apply_scale(4.0f)),
                ImVec2(sidebar_max.x + g_styling.apply_scale(5.0f), sidebar_max.y - g_styling.apply_scale(4.0f)),
                ImGui::ColorConvertFloat4ToU32(ImVec4(accent.x, accent.y, accent.z, 0.18f))
            );

            ImGui::SameLine(0.0f, g_styling.apply_scale(10.0f));

            // Right side: main content with fade
            ImGui::BeginChild("##right", ImVec2(0, 0), true);

            if (category != s_lastCategory)
            {
                s_pageSlideDir = category > s_lastCategory ? 1.0f : -1.0f;
                s_fadeAnim = 0.0f;
                s_lastCategory = category;
            }

            ImGuiIO& io = ImGui::GetIO();
            const float fade_speed = 13.0f;
            s_fadeAnim += (1.0f - s_fadeAnim) * ImClamp(fade_speed * io.DeltaTime, 0.0f, 1.0f);
            if (s_fadeAnim > 0.999f)
                s_fadeAnim = 1.0f;

            const float eased = ease_out_cubic(s_fadeAnim);
            const float slide_x = (1.0f - eased) * s_pageSlideDir * g_styling.apply_scale(22.0f);
            const float slide_y = (1.0f - eased) * g_styling.apply_scale(8.0f);

            {
                const ImVec2 glow_min = ImGui::GetWindowPos();
                const ImVec2 glow_max = glow_min + ImGui::GetWindowSize();
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImLerp(0.35f, 1.0f, eased) * ImGui::GetStyle().Alpha);
                Widgets::DrawParallaxBackgroundGlows(glow_min, glow_max, accent);
                ImGui::PopStyleVar();
            }

            draw_active_panel(category, eased, slide_x, slide_y);

            ImGui::EndChild();
            ImGui::End();
        }

        static void DrawVisualsPanel()
        {
            Widgets::BeginPanelColumns("##visuals-columns");

            if (Widgets::BeginSection("General"))
            {
                Widgets::Checkbox(encrypt("Lines"), &External::Config::g_lines_enabled);
                Widgets::Checkbox(encrypt("Boxes"), &External::Config::g_boxes_enabled);
            }
            Widgets::EndSection();

            if (Widgets::BeginSection("Tuning"))
            {
                Widgets::SliderFloat(encrypt("Snaplines Orgin"),
                    &External::Config::g_lines_origin, 0, 1);

                Widgets::SliderFloat(encrypt("Snaplines Thickness"),
                    &External::Config::g_line_thickness, 0, 5);

                Widgets::SliderFloat(encrypt("Box Thickness"),
                    &External::Config::g_box_thickness, 0, 5);
            }
            Widgets::EndSection();

            ImGui::NextColumn();

            if (Widgets::BeginSection("Colors"))
            {
                ImGui::ColorEdit4(encrypt("Teammate"), (float*)&External::Config::g_teammate_color);
                ImGui::ColorEdit4(encrypt("Tank"), (float*)&External::Config::g_tank_color);
                ImGui::ColorEdit4(encrypt("Plane"), (float*)&External::Config::g_plane_color);
            }
            Widgets::EndSection();

            if (Widgets::BeginSection("Details"))
            {
                Widgets::Checkbox(encrypt("Usernames"), &External::Config::g_usernames_enabled);
                Widgets::Checkbox(encrypt("Distance"), &External::Config::g_distance_enabled);
            }
            Widgets::EndSection();

            Widgets::EndPanelColumns();
        }

        static void DrawSettingsPanel()
        {
            Widgets::BeginPanelColumns("##settings-columns");

            if (Widgets::BeginSection("Overlay"))
            {
                Widgets::Checkbox(encrypt("VSync"), &External::Config::g_using_vsync);
                Widgets::Checkbox(encrypt("Stream Mode"), &External::Config::g_is_streamer_mode);
            }
            Widgets::EndSection();

            if (Widgets::BeginSection("Theme"))
            {
                Widgets::ColorEdit4(encrypt("Accent Color"), &External::Config::g_menu_accent_color);
            }
            Widgets::EndSection();

            ImGui::NextColumn();

            if (Widgets::BeginSection("Performance"))
            {
                Widgets::Checkbox(encrypt("Console Window"), &External::Config::g_using_console);
                Widgets::Checkbox(encrypt("Efficiency Mode"), &External::Config::g_using_efficiency_mode);
            }
            Widgets::EndSection();

            Widgets::EndPanelColumns();
        }
	};

    inline c_menu g_menu{};
}