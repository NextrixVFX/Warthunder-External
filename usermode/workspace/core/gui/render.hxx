#pragma once

namespace External::Render
{
	class c_render
	{
	public:
        static inline framerate_counter_t m_framerate_counter{};
		static inline ID3D11Device* m_device = nullptr;
		static inline ID3D11DeviceContext* m_device_context = nullptr;
		static inline IDXGISwapChain* m_swap_chain = nullptr;
		static inline ID3D11RenderTargetView* m_render_target = nullptr;
        static inline bool m_running = true;

        static void destroy_device()
        {
            m_running = false;
            sleep_short(100);

            ::ImGui_ImplDX11_Shutdown();
            ::ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            if (m_device_context) { m_device_context->Release(); m_device_context = nullptr; }
            if (m_render_target) { m_render_target->Release();  m_render_target = nullptr; }
            if (m_swap_chain) { m_swap_chain->Release();     m_swap_chain = nullptr; }
            if (m_device) { m_device->Release();         m_device = nullptr; }

            Assets::g_asset_loader.shutdown();
            g_styling.m_fonts = {};

            Logger::print(encrypt("[c_render] stop_renderer: Stopped rendering thread."));
        }

        static bool recreate_render_target()
        {
            if (!m_swap_chain || !m_device)
                return false;

            if (m_render_target)
            {
                m_render_target->Release();
                m_render_target = nullptr;
            }

            ID3D11Texture2D* back_buffer = nullptr;
            if (FAILED(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))) || !back_buffer)
                return false;

            const HRESULT hr = m_device->CreateRenderTargetView(back_buffer, nullptr, &m_render_target);
            back_buffer->Release();
            return SUCCEEDED(hr);
        }

        static void resize_swap_chain(int width, int height)
        {
            if (!m_swap_chain || width <= 0 || height <= 0)
                return;

            if (m_device_context)
                m_device_context->OMSetRenderTargets(0, nullptr, nullptr);

            if (m_render_target)
            {
                m_render_target->Release();
                m_render_target = nullptr;
            }

            if (FAILED(m_swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
                return;

            recreate_render_target();
        }

        static void apply_ui_scale(float scale)
        {
            if (scale < 0.75f)
                scale = 0.75f;
            if (scale > 2.5f)
                scale = 2.5f;

            static float applied = 0.f;
            if (std::fabs(scale - applied) < 0.02f)
                return;

            ImGui::GetIO().FontGlobalScale = scale;

            g_styling.set_scale(scale);
            Common::g_ui_scale = scale;
            applied = scale;
        }

        static float compute_ui_scale(int /*phys_w*/, int phys_h)
        {
            float res_scale = 1.f;
            if (phys_h > 0)
            {
                res_scale = static_cast<float>(phys_h) / 1080.f;
                if (res_scale < 1.f)
                    res_scale = 1.f;
                if (res_scale > 2.5f)
                    res_scale = 2.5f;
            }
            return res_scale;
        }

    private:
        static inline bool create_device()
        {
            if (!g_window.m_window)
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
            m_desc.OutputWindow = g_window.m_window;
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

            Assets::g_asset_loader.set_device(m_device);
            Assets::g_asset_loader.initialize(io, g_styling.m_fonts);

            if (!::ImGui_ImplWin32_Init(g_window.m_window) || !::ImGui_ImplDX11_Init(m_device, m_device_context))
            {
                Logger::print(encrypt("imgui backend init failed"));
                destroy_device();
                return false;
            }

            RECT overlay_rect{};
            ::GetClientRect(g_window.m_window, &overlay_rect);
            const int overlay_w = overlay_rect.right - overlay_rect.left;
            const int overlay_h = overlay_rect.bottom - overlay_rect.top;
            Common::update_screen_metrics(overlay_w, overlay_h);
            apply_ui_scale(compute_ui_scale(overlay_w, overlay_h));

            return true;
        }

        static void render_frame(ImGuiIO& io)
        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (GetAsyncKeyState(VK_RSHIFT) & 1)
                g_menu.menu_active = !g_menu.menu_active;

            g_shapes.get_draw_list() = ImGui::GetBackgroundDrawList();
            Features::on_render();

            if (g_menu.menu_active)
            {
                g_menu.render_menu();
            }

            g_window.update_capturability();
            g_menu.update_visibility();

            ImGui::Render();

            static const float m_clear[4] = { 0.0f , 0.0f , 0.0f , 0.0f };
            m_device_context->ClearRenderTargetView(m_render_target, m_clear);
            m_device_context->OMSetRenderTargets(1, &m_render_target, nullptr);

            ImDrawData* draw_data = ImGui::GetDrawData();
            draw_data->ScaleClipRects(draw_data->FramebufferScale);
            ImGui_ImplDX11_RenderDrawData(draw_data);

            m_swap_chain->Present(static_cast<int>(Config::g_using_vsync), 0);
        }

        static void clear_frame()
        {
            static const float m_clear[4] = { 0.0f , 0.0f , 0.0f , 0.0f };
            if (m_device_context) [[likely]]
            {
                m_device_context->ClearRenderTargetView(m_render_target, m_clear);
                m_device_context->OMSetRenderTargets(1, &m_render_target, nullptr);
                m_swap_chain->Present(static_cast<int>(Config::g_using_vsync), 0);
            }
        }

    public:
        static void start_rendering(const std::string& window_name)
        {
            ImGui_ImplWin32_EnableDpiAwareness();

            RECT m_rect_cache{ 0 , 0 , 0 , 0 };
            MSG  m_msg{ 0 };

            c_window::m_game_window = Common::find_window(window_name.c_str());

            while (m_running)
            {
                MSG msg;
                while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                HWND m_foreground = ::GetForegroundWindow();
                Config::g_is_rendering = (m_foreground == g_window.m_game_window || m_foreground == g_window.m_window);

                if (Config::g_is_rendering)
                {
                    if (Config::g_using_efficiency_mode)
                        Common::sleep_short(1);

                    RECT client_rect{};
                    POINT client_origin{ 0, 0 };
                    ::GetClientRect(g_window.m_game_window, &client_rect);
                    ::ClientToScreen(g_window.m_game_window, &client_origin);

                    int phys_w = client_rect.right - client_rect.left;
                    int phys_h = client_rect.bottom - client_rect.top;

                    Common::update_screen_metrics(phys_w, phys_h);
                    apply_ui_scale(compute_ui_scale(phys_w, phys_h));

                    RECT new_rect{};
                    new_rect.left = client_origin.x;
                    new_rect.top = client_origin.y;
                    new_rect.right = client_origin.x + phys_w;
                    new_rect.bottom = client_origin.y + phys_h;

                    g_window.m_game_rect.left = client_origin.x;
                    g_window.m_game_rect.top = client_origin.y;

                    if (std::memcmp(&new_rect, &m_rect_cache, sizeof(RECT)) != 0)
                    {
                        m_rect_cache = new_rect;
                        g_window.m_pos_x = client_origin.x;
                        g_window.m_pos_y = client_origin.y;
                        g_window.m_render_width = phys_w;
                        g_window.m_render_height = phys_h;

                        SetWindowPos(
                            g_window.m_window,
                            nullptr,
                            client_origin.x,
                            client_origin.y,
                            phys_w,
                            phys_h,
                            SWP_NOREDRAW
                        );

                        resize_swap_chain(phys_w, phys_h);
                    }

                    ImGuiIO& m_io = ImGui::GetIO();
                    m_io.DeltaTime = 1.0f / 120.0f;

                    POINT m_cursor{};
                    if (::GetCursorPos(&m_cursor))
                    {
                        ::ScreenToClient(g_window.m_window, &m_cursor);
                        m_io.MousePos.x = static_cast<float>(m_cursor.x);
                        m_io.MousePos.y = static_cast<float>(m_cursor.y);
                    }

                    m_io.MouseDown[0] = (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

                    render_frame(m_io);

                    HWND m_prev = ::GetWindow(m_foreground, GW_HWNDPREV);
                    if (m_foreground == g_window.m_window)
                        m_prev = ::GetWindow(g_window.m_game_window, GW_HWNDPREV);

                    ::SetWindowPos(g_window.m_window, m_prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
                else
                {
                    if (Config::g_is_rendering)
                        Config::g_is_rendering = false;
                    
                    clear_frame();
                    Common::sleep_short(100);
                }

                m_framerate_counter.tick();
            }
        }

        // note: creates a thread
        static void initialize(const std::string& window_name)
        {
            std::thread([window_name] {
				if (!g_window.create_window_binding(window_name))
                	return;
				
				if (!create_device())
                	return;
				
				start_rendering(window_name);
			}).detach();
        }
	};

    inline c_render g_render{};
}
