#pragma once

namespace External::Features
{
	// used to define the identity of features
	enum FEATURE_ID : int8_t
	{
		none,
		visual_esp
	};

	// will be implemented in each feature
	class c_feature
	{
	public:
		FEATURE_ID m_identifier = FEATURE_ID::none;
		bool m_enabled = true;

		c_feature(FEATURE_ID id) : m_identifier(id)
		{ }

		~c_feature() = default;

		virtual void on_render() = 0;
		virtual void on_update() = 0;
	};

	inline std::vector<std::unique_ptr<c_feature>> feature_array;
	
	inline c_feature* get_feature(FEATURE_ID id)
	{
		for (auto& f : feature_array)
			if (f->m_identifier == id)
				return f.get();
		return nullptr;
	}

	inline void set_enabled(FEATURE_ID id, bool enabled)
	{
		if (auto* f = get_feature(id))
			f->m_enabled = enabled;
	}

	inline std::vector<std::function<std::unique_ptr<c_feature>()>> feature_registry;

	template<typename T>
	class c_feature_registrar
	{
	public:
		c_feature_registrar()
		{
			feature_registry.push_back([]()
			{
				return std::make_unique<T>();
			});
		}
	};

	inline void register_features()
	{
		for (auto& factory : feature_registry)
		{
			feature_array.push_back(factory());
		}

		Logger::print(encrypt("Registered all features."));
	}

	// called in render thread
	inline void on_render()
	{
		for (auto& feature : feature_array)
		{
			if (feature->m_enabled)
				feature->on_render();
		}
	}

	// called in main thread
	inline void on_update()
	{
		for (auto& feature : feature_array)
		{
			if (feature->m_enabled)
				feature->on_update();
		}
	}
}