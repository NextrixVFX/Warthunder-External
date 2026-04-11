#pragma once

namespace External::Render
{
	class c_menu
	{
    public:
        inline static HHOOK m_mouse_hook = nullptr;
        inline static bool m_mouse_hooked = false;
        static inline bool menu_active = true;

    private:
        inline static int s_lastCategory = 0;
        inline static float s_fadeAnim = 1.0f;

        inline static const char* _distMethods[] = { "Screen Distance", "World Distance" };
        inline static const char* _bones[] = { "Head", "Neck", "Chest", "Torso" };
        inline static const char* _chamsColors[] = { "Red", "Blue", "Yellow", "White" };
        inline static const char* _lowestRarity[] = { "Common", "Uncommon", "Rare", "Epic", "Legendary" };
        inline static const char* _boxTypes[] = { "Filled", "Corner", "Regular" };

        inline static bool menu_setup = false;

        static LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam) 
        {
            if (nCode >= 0) {
                if (wParam == WM_MOUSEWHEEL) {
                    MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
                    int delta = GET_WHEEL_DELTA_WPARAM(p->mouseData);
                    ImGui::GetIO().AddMouseWheelEvent(0, (float)delta / WHEEL_DELTA);
                }
            }

            return CallNextHookEx(m_mouse_hook, nCode, wParam, lParam);
        }

        static void update_visibility()
        {
            if (!c_window::m_window)
                return;

            LONG_PTR ex = GetWindowLongPtr(c_window::m_window, GWL_EXSTYLE);

            if (ImGui::GetIO().WantCaptureMouse)
            {
                if (!m_mouse_hooked)
                {
                    m_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook, GetModuleHandle(nullptr), 0);
                    m_mouse_hooked = true;
                }

                ex &= ~WS_EX_TRANSPARENT;
            }
            else
            {
                if (m_mouse_hooked)
                {
                    UnhookWindowsHookEx(m_mouse_hook);
                    m_mouse_hooked = false;
                }

                ex |= WS_EX_TRANSPARENT;
            }

            SetWindowLongPtr(c_window::m_window, GWL_EXSTYLE, ex);
        }

    public:
        static void setup_menu()
        {
            if (menu_setup)
                return;

            ImGuiStyle& style = ImGui::GetStyle();

            // Rounding
            style.WindowRounding = 6.0f;
            style.FrameRounding = 4.0f;
            style.GrabRounding = 4.0f;
            style.ScrollbarRounding = 4.0f;
            style.TabRounding = 4.0f;
            style.ChildRounding = 4.0f;

            // Spacing & Padding
            style.WindowPadding = ImVec2(8.0f, 8.0f);
            style.FramePadding = ImVec2(6.0f, 4.0f);
            style.ItemSpacing = ImVec2(8.0f, 6.0f);
            style.ItemInnerSpacing = ImVec2(8.0f, 4.0f);
            style.IndentSpacing = 20.0f;
            style.ScrollbarSize = 12.0f;
            style.GrabMinSize = 8.0f;

            // Borders
            style.WindowBorderSize = 1.0f;
            style.ChildBorderSize = 1.0f;
            style.FrameBorderSize = 0.0f;

            // Column spacing
            style.ColumnsMinSpacing = 12.0f;

            ImVec4* colors = style.Colors;

            // Window
            colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.10f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.13f, 1.00f);
            colors[ImGuiCol_Border] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

            // Title
            colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.08f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.12f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.08f, 0.75f);

            // Text
            colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.93f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.55f, 0.95f, 0.35f);

            // Frame (inputs, etc.)
            colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.18f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.17f, 0.24f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);

            // Checkmark & Sliders
            colors[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);

            // Buttons
            colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.26f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.42f, 1.00f);

            // Headers (collapsing headers, selectables)
            colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.26f, 1.00f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.42f, 1.00f);

            // Separators
            colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);

            // Tabs
            colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.11f, 0.16f, 1.00f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
            colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.18f, 0.26f, 1.00f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.09f, 0.13f, 1.00f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.20f, 1.00f);

            // Scrollbar
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.09f, 1.00f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.18f, 0.18f, 0.26f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.42f, 1.00f);

            // Resize grip
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.18f, 0.26f, 0.50f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.35f, 0.75f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);

            // Popup/Modal
            colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.13f, 0.95f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
            menu_setup = true;
        }

        static void render_menu()
        {
            setup_menu();

            ImGui::SetNextWindowSize(ImVec2(612, 320), ImGuiCond_Always);
            ImGui::Begin(encrypt("sonar v0.0.1 - private"), nullptr);

            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();

            ImDrawList* bg_draw_list = ImGui::GetBackgroundDrawList();

            int shadow_layers = 12;
            float shadow_offset = 0.0f;
            float shadow_spread = 10.0f;

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
                    IM_COL32(20, 20, 20, (int)(alpha * 255)),
                    6.0f + thickness * 0.3f
                );
            }

            // Left side: custom vertical tabs
            ImGui::BeginChild("##left", ImVec2(160, 0), true);

            static int category = 0;
            const char* tabs[] = { "Visuals", "Settings" };
            UI::CustomWidgets::VerticalTabs("##sidebar", tabs, 2, &category);

            ImGui::EndChild();

            ImGui::SameLine();

            // Right side: main content with fade
            ImGui::BeginChild("##right", ImVec2(0, 0), true);

            if (category != s_lastCategory)
            {
                s_fadeAnim = 0.0f;
                s_lastCategory = category;
            }

            ImGuiIO& io = ImGui::GetIO();
            const float fade_speed = 10.0f;
            s_fadeAnim += (1.0f - s_fadeAnim) * ImClamp(fade_speed * io.DeltaTime, 0.0f, 1.0f);
            if (s_fadeAnim > 0.99f)
                s_fadeAnim = 1.0f;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, s_fadeAnim);

            switch (category)
            {
            case 0: DrawVisualsPanel(); break;
            case 1: DrawSettingsPanel(); break;
            default: DrawVisualsPanel(); break;
            }

            ImGui::PopStyleVar();

            ImGui::EndChild();

            ImGui::End();

            update_visibility();
        }

        static void DrawVisualsPanel()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            ImGui::Indent(4.0f);
            ImGui::Columns(2, "##visuals-columns", false);

            UI::CustomWidgets::BeginSection("General");
            
            UI::CustomWidgets::Checkbox(encrypt("Lines"), &External::Config::g_lines_enabled);
            UI::CustomWidgets::Checkbox(encrypt("Boxes"), &External::Config::g_boxes_enabled);
            
            UI::CustomWidgets::EndSection();


            UI::CustomWidgets::BeginSection("Tuning");
            ImGui::Indent(12.0f);
            
            UI::CustomWidgets::SliderFloat(encrypt("Snaplines Orgin"),
                &External::Config::g_lines_origin, 0, 1);

            UI::CustomWidgets::SliderFloat(encrypt("Snaplines Thickness"),
                &External::Config::g_line_thickness, 0, 5);

            UI::CustomWidgets::SliderFloat(encrypt("Box Thickness"),
                &External::Config::g_box_thickness, 0, 5);
                
            ImGui::Unindent(12.0f);
            UI::CustomWidgets::EndSection();

            ImGui::NextColumn();
            ImGui::Unindent(8.0f);

            UI::CustomWidgets::BeginSection("Colors");
            ImGui::Indent(12.0f);
            
            ImGui::ColorEdit4(encrypt("Teammate"), (float*)&External::Config::g_teammate_color,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);
            ImGui::ColorEdit4(encrypt("Tank"), (float*)&External::Config::g_tank_color,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);
            ImGui::ColorEdit4(encrypt("Plane"), (float*)&External::Config::g_plane_color,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);
                
            ImGui::Unindent(12.0f);
            UI::CustomWidgets::EndSection();

            UI::CustomWidgets::BeginSection("Details");
            
            UI::CustomWidgets::Checkbox(encrypt("Usernames"), &External::Config::g_usernames_enabled);
            UI::CustomWidgets::Checkbox(encrypt("Distance"), &External::Config::g_distance_enabled);
            
            UI::CustomWidgets::EndSection();

            ImGui::EndColumns();
            ImGui::PopStyleVar();
        }

        static void DrawSettingsPanel()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            ImGui::Indent(4.0f);
            ImGui::Columns(2, "##settings-columns", false);

            UI::CustomWidgets::BeginSection("Overlay");
            
            UI::CustomWidgets::Checkbox(encrypt("VSync"), &External::Config::g_vsync_enabled);
            UI::CustomWidgets::Checkbox(encrypt("Capturable"), &External::Config::g_capturable_enabled);
            
            UI::CustomWidgets::EndSection();

            ImGui::NextColumn();
            ImGui::Unindent(8.0f);

            ImGui::EndColumns();
            ImGui::PopStyleVar();
        }
	};
}