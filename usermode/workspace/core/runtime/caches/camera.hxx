#pragma once

namespace External::Runtime
{
	struct camera_frame_t
	{
		Engine::Structs::camera_t m_camera_data{};
	};

	class c_camera_cache : public c_cache
	{
	private:
		static inline c_cache_registrar<c_camera_cache> m_registrar{};
		static inline c_engine_cache* m_engine_cache = 0;

	public:
		c_double_buffer<camera_frame_t> m_data{};

		c_camera_cache() : c_cache(CACHE_ID::camera) {}

		void tick() override
		{
			sleep_short(1);

			camera_frame_t frame{};
			uintptr_t base_address = g_memory.m_application.m_base_address;

			c_engine_cache::get(m_engine_cache);
			auto& engine_data = m_engine_cache->m_data.get();
			if (!g_memory.is_valid_address(engine_data.m_camera_controller))
			{
				return;
			}

			Engine::Structs::camera_t camera{};
			
			camera.m_view_matrix = g_memory.read<matrix4x4_t<float>>(engine_data.m_camera_controller + offsets::camera::g_view_matrix);
			camera.m_positon = g_memory.read<vector3_t<float>>(engine_data.m_camera_controller + offsets::camera::g_position);
			
			frame.m_camera_data = camera;
			m_data.update(frame);
		}

		template<typename T = c_camera_cache*>
		static void get(T*& ptr)
		{
			if (!ptr)
				ptr = reinterpret_cast<T*>(get_cache(CACHE_ID::camera));
		}
	};
}