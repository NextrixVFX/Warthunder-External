#pragma once

namespace External::Render
{
	class c_window
	{
	public:
		static inline MARGINS m_window_margins = { -1 };
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
		static HWND create_owned_window(RECT size)
		{
			static bool class_registered = false;
			if (!class_registered)
			{
				WNDCLASSEXA wc{};
				wc.cbSize = sizeof(WNDCLASSEXA);
				wc.lpfnWndProc = DefWindowProcA;
				wc.hInstance = GetModuleHandleA(nullptr);
				wc.lpszClassName = encrypt("classname");
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

				if (!RegisterClassExA(&wc))
				{
					Logger::print(encrypt("failed to register overlay window class"));
					return nullptr;
				}

				class_registered = true;
			}

			HWND m_window = CreateWindowExA(
				WS_EX_LAYERED |
				WS_EX_TRANSPARENT |
				WS_EX_TOOLWINDOW,
				encrypt("classname"),
				encrypt("window"),
				WS_POPUP,
				size.left,
				size.top,
				size.right - size.left,
				size.bottom - size.top,
				nullptr,
				nullptr,
				GetModuleHandleA(nullptr),
				nullptr
			);

			if (!m_window)
			{
				Logger::print(encrypt("failed to create overlay window"));
				return nullptr;
			}

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

	public:
		static bool create_window_binding(const std::string& window_name)
		{
			m_game_window = ::FindWindowA(NULL, window_name.c_str());
			if (!m_game_window)
			{
				Logger::print(encrypt("Failed to find game window:\t%s"), window_name.c_str());
				return false;
			}

			RECT rect = get_client_area_and_size(m_game_window);
			m_window = create_owned_window(rect);
			if (!m_window)
			{
				Logger::print(encrypt("Failed to find IME child window (\"IME\" / \"Default IME\")"));
				return false;
			}

			// memory.kernel->windowHandle = m_window;
			if (!::SetWindowPos(
				m_window,
				nullptr,
				rect.left,
				rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				SWP_NOZORDER))
			{
				Logger::print(encrypt("SetWindowPos failed for IME overlay"));
				return false;
			}

			if (!m_window)
			{
				Logger::print(encrypt("Overlay window handle is null after binding"));
				return false;
			}

			LONG_PTR ex = GetWindowLongPtr(m_window, GWL_EXSTYLE);
			SetWindowLongPtr(
				m_window,
				GWL_EXSTYLE,
				ex | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE
			);

			MARGINS window_margin{ -1 };
			::DwmExtendFrameIntoClientArea(m_window, &window_margin);
			::SetLayeredWindowAttributes(m_window, 0, 255, LWA_ALPHA);

			::UpdateWindow(m_window);
			::ShowWindow(m_window, SW_SHOW);
			return true;
		}

		static inline void refresh_window()
		{
			::SetWindowPos(
				m_window,
				nullptr,
				m_pos_x,
				m_pos_y,
				m_render_width > 0 ? m_render_width : screen_width,
				m_render_height > 0 ? m_render_height : screen_height,
				0
			);
		}

		static void update_capturability()
		{
			if (!c_window::m_window)
				return;

			DWORD isExcluded = (Config::g_capturable_enabled) ? WDA_NONE : WDA_EXCLUDEFROMCAPTURE;

			::SetWindowDisplayAffinity(c_window::m_window, isExcluded);
		}
	};
}