#pragma once

namespace External::Logger
{
	template<typename... T>
	static inline void print(const char* message, T... args) noexcept
	{
		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);

		tm local_tm;
		::localtime_s(&local_tm, &time);

		::printf(encrypt("[%02d/%02d/%04d %02d:%02d:%02d] "),
			local_tm.tm_mon + 1,
			local_tm.tm_mday,
			local_tm.tm_year + 1900,
			local_tm.tm_hour,
			local_tm.tm_min,
			local_tm.tm_sec);

		::printf(message, args...);
		::printf(encrypt("\n"));
	}

	static inline void print(const char* message, uint8_t level = 0) noexcept
	{
		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);

		tm local_tm;
		::localtime_s(&local_tm, &time);

		const char* prefix = "";

		if (level > 0)
		{
			prefix = (level == 1) ? "WARN" : "ERROR";
		}

		::printf(encrypt("[%02d/%02d/%04d %02d:%02d:%02d] %s %s\n"),
			local_tm.tm_mon + 1,
			local_tm.tm_mday,
			local_tm.tm_year + 1900,
			local_tm.tm_hour,
			local_tm.tm_min,
			local_tm.tm_sec,
			prefix,
			message);
	}

	static inline void warn(const char* message) noexcept
	{
		print(message, 1);
	}

	static inline void error(const char* message)
	{
		print(message, 2);
	}

	template<typename T>
	static inline void print_vector(std::string txt, External::Primitives::vector2_t<T> input)
	{
		constexpr int width = 12; // adjust spacing here

		std::cout << "[" << txt << "] "
			<< std::setw(width) << std::right << input.x
			<< std::setw(width) << std::right << input.y
			<< '\n';
	}

	template<typename T>
	static inline void print_vector(std::string txt, External::Primitives::vector3_t<T> input)
	{
		constexpr int width = 12; // adjust spacing here

		std::cout << "[" << txt << "] "
			<< std::setw(width) << std::right << input.x
			<< std::setw(width) << std::right << input.y
			<< std::setw(width) << std::right << input.z
			<< '\n';
	}

	template<typename T>
	static inline void print_vector(std::string txt, External::Primitives::vector4_t<T> input)
	{
		constexpr int width = 12; // adjust spacing here

		std::cout << "[" << txt << "] "
			<< std::setw(width) << std::right << input.x
			<< std::setw(width) << std::right << input.y
			<< std::setw(width) << std::right << input.z
			<< std::setw(width) << std::right << input.w
			<< '\n';
	}
};