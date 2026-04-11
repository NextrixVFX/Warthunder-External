#pragma once

namespace External::Runtime
{
	enum CACHE_ID : int8_t
	{
		none,
		engine,
		camera,
		entities
	};

	class c_cache
	{
	public:
		Common::s_framerate_counter m_fr_counter{};
		CACHE_ID m_identifier = CACHE_ID::none;
		bool m_running = false;
		
		c_cache(CACHE_ID id) : m_identifier(id)
		{ }

		~c_cache() = default;

		// implementation must include a sleep at the end!
		virtual void tick() = 0;
		
		void start()
		{
			if (m_identifier == CACHE_ID::none)
				return;

			std::thread([this] 
			{
				if (m_running)
					return;

				m_running = true;
				while (m_running)
				{
					tick();
					m_fr_counter.tick();
				}

			}).detach();
		}
	};

	inline std::vector<std::unique_ptr<c_cache>> cache_array;

	inline c_cache* get_cache(CACHE_ID id)
	{
		for (auto& f : cache_array)
			if (f->m_identifier == id)
				return f.get();

		return nullptr;
	}

	inline std::vector<std::function<std::unique_ptr<c_cache>()>> cache_registry;

	template<typename T>
	class c_cache_registrar
	{
	public:
		c_cache_registrar()
		{
			cache_registry.push_back([]()
			{
				return std::make_unique<T>();
			});
		}
	};

	inline void register_caches()
	{
		for (auto& factory : cache_registry)
		{
			cache_array.push_back(factory());
		}

		Logger::print(encrypt("Registered all caches."));
	}

	inline void start_caches()
	{
		for (auto& cache : cache_array)
		{
			cache->start();
		}

		Logger::print(encrypt("Started all caches."));
	}

	inline void stop_caches()
	{
		for (auto& cache : cache_array)
		{
			cache->m_running = false;
		}

		Logger::print(encrypt("Stopped all caches."));
	}
}