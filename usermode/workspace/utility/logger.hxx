#pragma once

#include <windows.h>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>

namespace External::Logger
{
    namespace detail
    {
        // Enable ANSI escape sequences on modern Windows consoles.
        static inline void initialize_console() noexcept
        {
            static bool initialized = false;
            if (initialized)
                return;

            initialized = true;

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hConsole == INVALID_HANDLE_VALUE)
                return;

            DWORD mode = 0;
            if (!GetConsoleMode(hConsole, &mode))
                return;

            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hConsole, mode);
        }

        // RGB color helper.
        static inline void set_rgb(uint8_t r, uint8_t g, uint8_t b) noexcept
        {
            std::printf("\x1b[38;2;%u;%u;%um", r, g, b);
        }

        static inline void reset_color() noexcept
        {
            std::printf("\x1b[0m");
        }

        // Linear interpolation helper.
        static inline uint8_t lerp_u8(uint8_t a, uint8_t b, float t) noexcept
        {
            return static_cast<uint8_t>(a + (b - a) * t);
        }

        // Prints text using a left->right RGB gradient.
        static inline void print_gradient(
            const char* text,
            // Start color: vibrant purple
            uint8_t r1 = 168, uint8_t g1 = 85, uint8_t b1 = 247,
            // End color: bright cyan-blue
            uint8_t r2 = 59, uint8_t g2 = 130, uint8_t b2 = 246
        ) noexcept
        {
            if (!text || !*text)
                return;

            const size_t len = std::strlen(text);
            if (len == 0)
                return;

            for (size_t i = 0; i < len; ++i)
            {
                float t = (len > 1)
                    ? static_cast<float>(i) / static_cast<float>(len - 1)
                    : 0.0f;

                uint8_t r = lerp_u8(r1, r2, t);
                uint8_t g = lerp_u8(g1, g2, t);
                uint8_t b = lerp_u8(b1, b2, t);

                set_rgb(r, g, b);
                std::putchar(text[i]);
            }

            reset_color();
        }
    }

    template<typename... T>
    static inline void print(const char* message, T... args) noexcept
    {
        detail::initialize_console();

        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        tm local_tm;
        ::localtime_s(&local_tm, &time);

        // Timestamp in dim gray.
        detail::set_rgb(140, 140, 140);
        std::printf(encrypt("[%02d/%02d/%04d %02d:%02d:%02d] "),
            local_tm.tm_mon + 1,
            local_tm.tm_mday,
            local_tm.tm_year + 1900,
            local_tm.tm_hour,
            local_tm.tm_min,
            local_tm.tm_sec);
        detail::reset_color();

        // Format message first so we can apply gradient to the final text.
        char buffer[4096];
        std::snprintf(buffer, sizeof(buffer), message, args...);

        // Gradient colored message.
        detail::print_gradient(buffer);

        std::printf("\n");
    }

    static inline void print(const char* message, uint8_t level = 0) noexcept
    {
        detail::initialize_console();

        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        tm local_tm;
        ::localtime_s(&local_tm, &time);

        // Timestamp in dim gray.
        detail::set_rgb(140, 140, 140);
        std::printf(encrypt("[%02d/%02d/%04d %02d:%02d:%02d] "),
            local_tm.tm_mon + 1,
            local_tm.tm_mday,
            local_tm.tm_year + 1900,
            local_tm.tm_hour,
            local_tm.tm_min,
            local_tm.tm_sec);
        detail::reset_color();

        // Level prefix with fixed colors.
        if (level == 1)
        {
            detail::set_rgb(255, 215, 0); // Gold
            std::printf("[WARN] ");
            detail::reset_color();
        }
        else if (level == 2)
        {
            detail::set_rgb(255, 80, 80); // Red
            std::printf("[ERROR] ");
            detail::reset_color();
        }

        // Message gradient.
        detail::print_gradient(message);

        std::printf("\n");
    }

    static inline void warn(const char* message) noexcept
    {
        print(message, 1);
    }

    static inline void error(const char* message) noexcept
    {
        print(message, 2);
    }

    template<typename T>
    static inline void print_vector(std::string txt, External::Primitives::vector2_t<T> input)
    {
        constexpr int width = 12;

        std::cout << "[" << txt << "] "
            << std::setw(width) << std::right << input.x
            << std::setw(width) << std::right << input.y
            << '\n';
    }

    template<typename T>
    static inline void print_vector(std::string txt, External::Primitives::vector3_t<T> input)
    {
        constexpr int width = 12;

        std::cout << "[" << txt << "] "
            << std::setw(width) << std::right << input.x
            << std::setw(width) << std::right << input.y
            << std::setw(width) << std::right << input.z
            << '\n';
    }

    template<typename T>
    static inline void print_vector(std::string txt, External::Primitives::vector4_t<T> input)
    {
        constexpr int width = 12;

        std::cout << "[" << txt << "] "
            << std::setw(width) << std::right << input.x
            << std::setw(width) << std::right << input.y
            << std::setw(width) << std::right << input.z
            << std::setw(width) << std::right << input.w
            << '\n';
    }
};