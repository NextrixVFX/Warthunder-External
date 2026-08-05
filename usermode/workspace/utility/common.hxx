#pragma once

namespace External::Common
{
	inline static bool g_console_initialized = false;

	inline static int g_screen_width = 0;
	inline static int g_screen_height = 0;

	inline static Primitives::vector2_t<float> g_screen_size{};
	inline static Primitives::vector2_t<float> g_screen_center{};
	inline static float g_ui_scale = 1.f;

	inline void toggle_console(bool show)
	{
		if (show)
		{
			if (!g_console_initialized)
			{
				AllocConsole();

				FILE* fp;

				freopen_s(&fp, "CONOUT$", "w", stdout);
				freopen_s(&fp, "CONOUT$", "w", stderr);
				freopen_s(&fp, "CONIN$", "r", stdin);

				std::cout.clear();
				std::cerr.clear();
				std::cin.clear();

				g_console_initialized = true;
			}

			ShowWindow(GetConsoleWindow(), SW_SHOW);
		}
		else
		{
			HWND console = GetConsoleWindow();

			if (console)
				ShowWindow(console, SW_HIDE);
		}
	}

	inline static HWND find_window(const std::string& text)
	{
		struct Data
		{
			const std::string* search;
			HWND result = nullptr;
		} data{ &text };

		EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
			{
				auto* data = reinterpret_cast<Data*>(lParam);

				if (!IsWindowVisible(hwnd))
					return TRUE;

				char title[512]{};
				GetWindowTextA(hwnd, title, sizeof(title));

				if (std::string(title).find(*data->search) != std::string::npos)
				{
					data->result = hwnd;
					return FALSE;
				}

				return TRUE;
			}, reinterpret_cast<LPARAM>(&data));

		return data.result;
	}

	inline static void update_screen_metrics(int width, int height)
	{
		if (width <= 0 || height <= 0)
			return;

		g_screen_width = width;
		g_screen_height = height;
		g_screen_size = { static_cast<float>(width), static_cast<float>(height) };
		g_screen_center = g_screen_size * 0.5f;
	}

	inline static RECT get_client_area_and_size(HWND hwnd)
	{
		RECT rect{ };
		if (GetClientRect(hwnd, &rect))
		{
			POINT top_left{ rect.left ,  rect.top };
			POINT bottom_right{ rect.right , rect.bottom };

			ClientToScreen(hwnd, &top_left);
			ClientToScreen(hwnd, &bottom_right);

			rect.left = top_left.x;
			rect.top = top_left.y;
			rect.right = bottom_right.x;
			rect.bottom = bottom_right.y;
		}
		else
		{
			rect = { 0 , 0 , 0 , 0 };
		}

		return rect;
	}

	inline std::string random_string(size_t length)
	{
		const std::string chars = encrypt("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, chars.size() - 1);

		std::string result;
		result.reserve(length);

		for (size_t i = 0; i < length; i++)
			result += chars[dist(gen)];

		return result;
	}

	inline void sleep_short(float milliseconds)
	{
		static bool once = true;
		if (once) {
			ULONG actualResolution;
			ZwSetTimerResolution(1, true, &actualResolution);
			once = false;
		}

		LARGE_INTEGER interval{};
		interval.QuadPart = static_cast<int>(-1) * (static_cast<int>(milliseconds) * 10000.0f);
		NtDelayExecution(false, &interval);
	}

	struct framerate_counter_t
	{
		std::atomic<float> m_fr{ 0.0f };

		int m_frames = 0;
		std::chrono::high_resolution_clock::time_point m_last_time =
			std::chrono::high_resolution_clock::now();

		inline void tick()
		{
			m_frames++;

			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = now - m_last_time;

			if (diff.count() >= 1.0)
			{
				m_fr.store(static_cast<float>(m_frames) / diff.count(), std::memory_order_relaxed);

				m_frames = 0;
				m_last_time = now;
			}
		}
	};

	template<typename T, size_t N = 0>
	class c_double_buffer {
	private:
		using buffer = T;
		std::array<buffer, 2> m_buffers;
		std::atomic<int> m_front_index{ 0 };

	public:
		c_double_buffer()
		{
			if constexpr (N)
				for (auto& b : m_buffers)
					b.reserve(N);
		}

		const buffer& get()
		{
			return m_buffers[m_front_index.load(std::memory_order_acquire)];
		}

		const buffer get_copy()
		{
			return m_buffers[m_front_index.load(std::memory_order_relaxed)];
		}

		buffer& begin_write()
		{
			const int front = m_front_index.load(std::memory_order_relaxed);
			auto& back = m_buffers[1 - front];

			if constexpr (N) back.clear();
			else back = {};

			return back;
		}

		void end_write()
		{
			const int front = m_front_index.load(std::memory_order_relaxed);
			m_front_index.store(1 - front, std::memory_order_release);
		}

		void update(const buffer& data)
		{
			auto& back = begin_write();
			back = data;
			end_write();
		}

		void update(buffer&& data)
		{
			auto& back = begin_write();
			back = std::move(data);
			end_write();
		}
	};
}