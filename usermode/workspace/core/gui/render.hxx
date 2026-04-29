#pragma once

namespace External::Render
{
	class c_render
	{
	public:
		static inline ID3D11Device* m_device = nullptr;
		static inline ID3D11DeviceContext* m_device_context = nullptr;
		static inline IDXGISwapChain* m_swap_chain = nullptr;
		static inline ID3D11RenderTargetView* m_render_target = nullptr;

        static inline struct s_fonts
        {
            ImFont* font_16{ };
            ImFont* font_20{ };
            ImFont* font_24{ };
            ImFont* font_30{ };
        } m_fonts;

        static void destroy_device()
        {
            if (m_device_context) { m_device_context->Release(); m_device_context = nullptr; }
            if (m_render_target) { m_render_target->Release();  m_render_target = nullptr; }
            if (m_swap_chain) { m_swap_chain->Release();     m_swap_chain = nullptr; }
            if (m_device) { m_device->Release();         m_device = nullptr; }
			
            if (c_menu::m_mouse_hooked && c_menu::m_mouse_hook) {
                UnhookWindowsHookEx(c_menu::m_mouse_hook);
                c_menu::m_mouse_hook = nullptr;
            }

            ::ImGui_ImplDX11_Shutdown();
            ::ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

    private:
        inline static ImFont* addFont(ImGuiIO& io, float size)
        {
            return io.Fonts->AddFontFromFileTTF(encrypt("assets\\Poppins.ttf"), size);
        }

        static inline bool create_device()
        {
            if (!c_window::m_window)
            {
                Logger::print(encrypt("create_device called with null overlay window"));
                return false;
            }

            DXGI_SWAP_CHAIN_DESC m_desc{ };
            m_desc.BufferCount = 2;
            m_desc.BufferDesc.Width = 0;
            m_desc.BufferDesc.Height = 0;
            m_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            m_desc.BufferDesc.RefreshRate.Numerator = 360;
            m_desc.BufferDesc.RefreshRate.Denominator = 1;
            m_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            m_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            m_desc.OutputWindow = c_window::m_window;
            m_desc.SampleDesc.Count = 1;
            m_desc.SampleDesc.Quality = 0;
            m_desc.Windowed = TRUE;
            m_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            const D3D_FEATURE_LEVEL m_levels[2] = { D3D_FEATURE_LEVEL_11_0 , D3D_FEATURE_LEVEL_10_0 };
            D3D_FEATURE_LEVEL       m_level{ };

            const HRESULT m_create = ::D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                0,
                m_levels,
                2,
                D3D11_SDK_VERSION,
                &m_desc,
                &m_swap_chain,
                &m_device,
                &m_level,
                &m_device_context
            );

            if (FAILED(m_create))
            {
                Logger::print(encrypt("d3d11 create device and swapchain failed"));
                destroy_device();
                return false;
            }

            Logger::print(encrypt("d3d11 device created successfully"));

            ID3D11Texture2D* m_back_buffer = nullptr;
            if (FAILED(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&m_back_buffer))) || !m_back_buffer)
            {
                Logger::print(encrypt("failed to get backbuffer from swapchain"));
                destroy_device();
                return false;
            }

            if (FAILED(m_device->CreateRenderTargetView(m_back_buffer, nullptr, &m_render_target)))
            {
                Logger::print(encrypt("failed to create render target view"));
                m_back_buffer->Release();
                destroy_device();
                return false;
            }

            m_back_buffer->Release();

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            m_fonts.font_16 = addFont(io, 16.0f);
            m_fonts.font_20 = addFont(io, 20.0f);
            m_fonts.font_24 = addFont(io, 24.0f);
            m_fonts.font_30 = addFont(io, 30.0f);

            if (!::ImGui_ImplWin32_Init(c_window::m_window) || !::ImGui_ImplDX11_Init(m_device, m_device_context))
            {
                Logger::print(encrypt("imgui backend init failed"));
                destroy_device();
                return false;
            }

            return true;
        }

        static void render_frame(ImGuiIO& io)
        {
            c_window::update_capturability();

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (::GetAsyncKeyState(VK_F2) & 0x8000)
                c_window::refresh_window();

            if (GetAsyncKeyState(VK_RSHIFT) & 1)
                c_menu::menu_active = !c_menu::menu_active;

            inst_shapes->get_draw_list() = ImGui::GetBackgroundDrawList();
            Features::on_render();

            if (c_menu::menu_active)
            {
                c_menu::render_menu();
            }

            ImGui::Render();

            static const float m_clear[4] = { 0.0f , 0.0f , 0.0f , 0.0f };
            m_device_context->ClearRenderTargetView(m_render_target, m_clear);
            m_device_context->OMSetRenderTargets(1, &m_render_target, nullptr);

            ImDrawData* draw_data = ImGui::GetDrawData();
            draw_data->ScaleClipRects(draw_data->FramebufferScale);
            ImGui_ImplDX11_RenderDrawData(draw_data);

            m_swap_chain->Present(Config::g_vsync_enabled, 0);
        }

        static void clear_frame()
        {
            static const float m_clear[4] = { 0.0f , 0.0f , 0.0f , 0.0f };
            m_device_context->ClearRenderTargetView(m_render_target, m_clear);
            m_device_context->OMSetRenderTargets(1, &m_render_target, nullptr);
            m_swap_chain->Present(Config::g_vsync_enabled, 0);
        }

    public:
        static void start_rendering(const std::string& window_name_1, const std::string& window_name_2)
        {
            ImGui_ImplWin32_EnableDpiAwareness();

            RECT m_rect_cache{ 0 , 0 , 0 , 0 };
            MSG  m_msg{ 0 };

            c_window::m_game_window = ::FindWindowA(NULL, window_name_1.c_str());

            if (!c_window::m_game_window)
                c_window::m_game_window = ::FindWindowA(NULL, window_name_2.c_str());

            while (true)
            {
                MSG msg;
                while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                HWND m_foreground = ::GetForegroundWindow();
                if (m_foreground == c_window::m_game_window || m_foreground == c_window::m_window) [[likely]]
                {
                    POINT m_client_origin{ };
                    std::memset(&c_window::m_game_rect, 0, sizeof(RECT));
                    ::GetClientRect(c_window::m_game_window, &c_window::m_game_rect);
                    ::ClientToScreen(c_window::m_game_window, &m_client_origin);

                    c_window::m_game_rect.left = m_client_origin.x;
                    c_window::m_game_rect.top = m_client_origin.y;

                    if (std::memcmp(&c_window::m_game_rect, &m_rect_cache, sizeof(RECT)) != 0) [[likely]]
                    {
                        m_rect_cache = c_window::m_game_rect;
                        c_window::m_pos_x = c_window::m_game_rect.left;
                        c_window::m_pos_y = c_window::m_game_rect.top;
                        c_window::m_window_width = c_window::m_game_rect.right;
                        c_window::m_window_height = c_window::m_game_rect.bottom;
                        c_window::m_render_width = c_window::m_game_rect.right;
                        c_window::m_render_height = c_window::m_game_rect.bottom;

                        ::SetWindowPos(
                            c_window::m_window,
                            nullptr,
                            m_client_origin.x,
                            m_client_origin.y,
                            c_window::m_render_width,
                            c_window::m_render_height,
                            SWP_NOREDRAW
                        );
                    }

                    ImGuiIO& m_io = ImGui::GetIO();
                    m_io.DeltaTime = 1.0f / 120.0f;

                    POINT m_cursor{ };
                    if (!::GetCursorPos(&m_cursor))
                    {
                        Logger::print(encrypt("failed to retrieve cursor position"));
                        continue;
                    }

                    ::ScreenToClient(c_window::m_window, &m_cursor);
                    m_io.MousePos.x = static_cast<float>(m_cursor.x);
                    m_io.MousePos.y = static_cast<float>(m_cursor.y);
                    m_io.MouseDown[0] = (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
                    m_io.MouseClicked[0] = m_io.MouseDown[0];

                    render_frame(m_io);

                    HWND m_prev = ::GetWindow(m_foreground, GW_HWNDPREV);
                    if (m_foreground == c_window::m_window)
                        m_prev = ::GetWindow(c_window::m_game_window, GW_HWNDPREV);

                    ::SetWindowPos(c_window::m_window, m_prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
                else
                {
                    clear_frame();

                    if (c_menu::m_mouse_hooked)
                    {
                        UnhookWindowsHookEx(c_menu::m_mouse_hook);
                        c_menu::m_mouse_hooked = false;
                    }

                    Common::sleep_short(100);
                }

                Common::sleep_short(1);
            }

            ::ImGui_ImplDX11_Shutdown();
            ::ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            ::DestroyWindow(c_window::m_window);
            destroy_device();
        }

        // note: creates a thread
        static void initialize(const std::string& window_name_1, const std::string& window_name_2)
        {
            std::thread([&] { 
				if (!c_window::create_window_binding(window_name_1, window_name_2))
                	return;
				
				if (!create_device())
                	return;
				
				start_rendering(window_name_1, window_name_2);
			}).detach();
        }
	};
}
