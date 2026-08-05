#pragma once

// remove this to do kernel
#define is_usermode_mem

#define STB_IMAGE_IMPLEMENTATION
#define WIN32_LEAN_AND_MEAN

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4005)
#pragma warning(disable: 4099)
#endif

#ifdef NDEBUG
#pragma comment(lib, "DirectXTK/Release/DirectXTK.lib")
#else
#pragma comment(lib, "DirectXTK/Debug/DirectXTK.lib") 
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "ntdll.lib")

// Json
#include <impl/include/rapidjson/document.h>
#include <impl/include/rapidjson/writer.h>
#include <impl/include/rapidjson/stringbuffer.h>
#include <impl/include/rapidjson/filereadstream.h>
#include <impl/include/rapidjson/filewritestream.h>

// General
#include <Windows.h>
#include <dbghelp.h>
#include <winternl.h>
#include <winhttp.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <thread>
#include <mutex>
#include <functional>
#include <shared_mutex>
#include <atomic>
#include <numbers>
#include <cstdint>
#include <random>
#include <print>
#include <chrono>
#include <array>
#include <bitset>
#include <stack>
#include <unordered_map>
#include <corecrt_math_defines.h>
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <dxgi.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <fstream>
#include <cstddef>
#include <winioctl.h>
#include <ntstatus.h>
#include <tlhelp32.h>
#include <wtypes.h>
#include <sysinfoapi.h>

extern "C" NTSTATUS NTAPI ZwSetTimerResolution(
	ULONG DesiredResolution,
	BOOLEAN SetResolution,
	PULONG CurrentResolution
);

extern "C" NTSTATUS NTAPI NtDelayExecution(
	BOOLEAN Alertable,
	PLARGE_INTEGER DelayInterval
);

// Etc Includes
#include <impl/include/SKCrypter/skcrypter.hpp>
#include <impl/include/DirectXTK/WICTextureLoader.h>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <impl/include/imgui/backends/imgui_impl_win32.h>
#include <impl/include/imgui/backends/imgui_impl_dx11.h>
#include <impl/include/imgui/imgui.h>
#include <impl/include/imgui/imgui_internal.h>

// Utility
#include <workspace/utility/primitives.hxx>
#include <workspace/utility/logger.hxx>
#include <workspace/utility/common.hxx>
#include <workspace/utility/assets.hxx>

using namespace External;
using namespace External::Common;
using namespace External::Primitives;

// Memory
#include <impl/memory/usermode/include.hxx>
#include <impl/memory/memory.hxx>

// Engine / SDK
#include <workspace/engine/offsets.hxx>
#include <workspace/engine/enums.hxx>
#include <workspace/engine/structs.hxx>
#include <workspace/engine/sdk.hxx>

using namespace External::Engine::Enums;
using namespace External::Engine::Structs;

// Config uses enums
#include <workspace/utility/config.hxx>

// Core Runtime Driver
#include <workspace/resources/resource.h>

// Core Runtime Cache
#include <workspace/core/runtime/caches/global.hxx>
#include <workspace/core/runtime/caches/registry.hxx>
#include <workspace/core/runtime/caches/engine.hxx>
#include <workspace/core/runtime/caches/camera.hxx>
#include <workspace/core/runtime/caches/entities.hxx>

// Core Gui
#include <workspace/core/features/registry.hxx>
#include <workspace/core/gui/deps/utility.hxx>
#include <workspace/core/gui/deps/styling.hxx>
#include <workspace/core/gui/deps/shapes.hxx>
#include <workspace/core/gui/deps/widgets.hxx>
#include <workspace/core/gui/window.hxx>
#include <workspace/core/gui/menu.hxx>
#include <workspace/core/gui/render.hxx>

// Core Features
#include <workspace/core/features/visual/esp.hxx>