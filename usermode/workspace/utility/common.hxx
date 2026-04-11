#pragma once

namespace External::Common
{
	inline static int screen_width = GetSystemMetrics(SM_CXSCREEN);
	inline static int screen_height = GetSystemMetrics(SM_CYSCREEN);

	inline static Primitives::vector2_t<float> screen_center = {
		screen_width * .5f,
		screen_height * .5f,
	};

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

	struct s_framerate_counter
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