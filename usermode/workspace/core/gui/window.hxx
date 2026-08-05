#pragma once

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace External::Render
{
	class c_window
	{
	public:
		static inline MARGINS m_window_margins = { -1 };
		static inline HWND m_task_manager = nullptr;
		static inline HWND m_window = nullptr;
		static inline HWND m_game_window = nullptr;
		static inline RECT m_game_rect{ };
		static inline int m_pos_x = 0;
		static inline int m_pos_y = 0;
		static inline int m_window_width = 0;
		static inline int m_window_height = 0;
		static inline int m_render_width = 0;
		static inline int m_render_height = 0;

	private:
		static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
		{
			if (ImGui::GetCurrentContext() && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
				return true;
			return DefWindowProcA(hwnd, msg, wp, lp);
		}

		static HWND create_owned_window(RECT size)
		{
			static bool class_registered = false;
			if (!class_registered)
			{
				WNDCLASSEXA wc{};
				wc.cbSize = sizeof(WNDCLASSEXA);
				wc.lpfnWndProc = wnd_proc;
				wc.hInstance = GetModuleHandleA(nullptr);
				wc.lpszClassName = encrypt("WorkerW");
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

				if (!RegisterClassExA(&wc))
				{
					Logger::print(encrypt("[c_window] create_owned_window: failed to register overlay window class"));
					return nullptr;
				}

				class_registered = true;
			}

			HWND m_window = CreateWindowExA(
				WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
				encrypt("WorkerW"), nullptr,
				WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
				size.left, size.top,
				size.right - size.left,
				size.bottom - size.top,
				nullptr, nullptr,
				GetModuleHandleA(nullptr), nullptr
			);

			if (!m_window)
			{
				Logger::print(encrypt("[c_window] create_owned_window: failed to create overlay window"));
				return nullptr;
			}

			LONG_PTR ex = GetWindowLongPtr(m_window, GWL_EXSTYLE);
			SetWindowLongPtrA(m_window, GWL_EXSTYLE, ex | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
			SetWindowTextA(m_window, encrypt("Taskmgr"));

			MARGINS margins{ -1, -1, -1, -1 };
			DwmExtendFrameIntoClientArea(m_window, &margins);
			SetLayeredWindowAttributes(m_window, 0, 255, LWA_ALPHA);

			BOOL use_dark_mode = TRUE;
			DwmSetWindowAttribute(m_window, DWMWA_USE_IMMERSIVE_DARK_MODE, &use_dark_mode, sizeof(use_dark_mode));

			BOOL disable = FALSE;
			DwmSetWindowAttribute(m_window, DWMWA_HAS_ICONIC_BITMAP, &disable, sizeof(disable));
			DwmSetWindowAttribute(m_window, DWMWA_FORCE_ICONIC_REPRESENTATION, &disable, sizeof(disable));
			DwmSetWindowAttribute(m_window, DWMWA_DISALLOW_PEEK, &disable, sizeof(disable));
			DwmSetWindowAttribute(m_window, DWMWA_EXCLUDED_FROM_PEEK, &disable, sizeof(disable));

			return m_window;
		}

		static HWND get_window_handle(std::uint32_t pid) {
			std::pair<HWND, DWORD> params = { 0, pid };
			auto result = EnumWindows([](HWND hwnd, LPARAM lParam) -> int {
				auto params = (std::pair<HWND, DWORD>*)(lParam);

				DWORD process_id;
				if (GetWindowThreadProcessId(hwnd, &process_id) && process_id == params->second) {

					SetLastError(-1);
					params->first = hwnd;
					return false;
				}

				return true;

				}, reinterpret_cast<LPARAM>(&params));

			if (!result && GetLastError() == -1 && params.first) {
				return params.first;
			}

			return 0;
		}

		static inline bool protect_overlay_window(HWND hwnd)
		{
			// implement overlay protections
			return true;
		}

	public:
		static bool create_window_binding(const std::string& window_name)
		{
			::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

			while (!m_game_window)
			{
				m_game_window = Common::find_window(window_name);

				if (!m_game_window)
					sleep_short(100);
			}
			
			Logger::print(encrypt("[c_window] create_window_binding: Found game window."));
			
			// Get the game window's physical client rect in screen coords
			RECT client_rect{};
			::GetClientRect(m_game_window, &client_rect);

			POINT origin{ 0, 0 };
			::ClientToScreen(m_game_window, &origin);

			RECT screen_rect{};
			screen_rect.left = origin.x;
			screen_rect.top = origin.y;
			screen_rect.right = origin.x + (client_rect.right - client_rect.left);
			screen_rect.bottom = origin.y + (client_rect.bottom - client_rect.top);

			m_window = create_owned_window(screen_rect);
			if (!m_window)
			{
				Logger::print(encrypt("[c_window] create_window_binding: Failed to find overlay window"));
				return false;
			}

			if (!SetWindowPos(
				m_window,
				nullptr,
				screen_rect.left,
				screen_rect.top,
				screen_rect.right - screen_rect.left,
				screen_rect.bottom - screen_rect.top,
				SWP_NOZORDER))
			{
				Logger::print(encrypt("[c_window] create_window_binding: SetWindowPos failed for IME overlay"));
				return false;
			}

			ShowWindow(m_window, SW_SHOW);
			UpdateWindow(m_window);

			RECT bound_client{};
			::GetClientRect(m_window, &bound_client);
			Common::update_screen_metrics(
				bound_client.right - bound_client.left,
				bound_client.bottom - bound_client.top
			);

			RAWINPUTDEVICE rid[2]{};
			rid[0].usUsagePage = 0x01;
			rid[0].usUsage = 0x02;
			rid[0].dwFlags = RIDEV_INPUTSINK;
			rid[0].hwndTarget = m_window;
			rid[1].usUsagePage = 0x01;
			rid[1].usUsage = 0x06;
			rid[1].dwFlags = RIDEV_INPUTSINK;
			rid[1].hwndTarget = m_window;
			RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));

			if (!protect_overlay_window(m_window))
			{
				Logger::print(encrypt("[c_window] create_window_binding: Failed to hide overlay window"));
				return false;
			}

			return true;
		}

		static void update_capturability()
		{
			if (!m_window)
				return;

			SetWindowDisplayAffinity(m_window, Config::g_is_streamer_mode ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
		}
	};

	inline c_window g_window{};
}