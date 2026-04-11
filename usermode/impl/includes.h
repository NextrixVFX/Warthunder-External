#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define WIN32_LEAN_AND_MEAN

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4005)
#endif

#pragma comment(lib, "version.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "usp10.lib")      
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "DirectXTK.lib") 
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "freetype.lib")

// Json
#include <include/rapidjson/document.h>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/stringbuffer.h>
#include <include/rapidjson/filereadstream.h>
#include <include/rapidjson/filewritestream.h>

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

extern "C" {
	extern "C" NTSTATUS NTAPI ZwSetTimerResolution(
		ULONG DesiredResolution,
		BOOLEAN SetResolution,
		PULONG CurrentResolution
	);

	extern "C" NTSTATUS NTAPI NtDelayExecution(
		BOOLEAN Alertable,
		PLARGE_INTEGER DelayInterval
	);
}

// Etc Includes
#include <include/stb/stb_image.h>
#include <include/SKCrypter/skcrypter.hpp>


// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <include/imgui/imgui.h>
#include <include/imgui/backends/imgui_impl_win32.h>
#include <include/imgui/backends/imgui_impl_dx11.h>
#include <include/imgui/imgui_internal.h>
#include <include/imgui/imgui_freetype.h>
#include <include/imgui/imgui_settings.h>
#include <include/imgui/imstb_textedit.h>

// Utility
#include <workspace/utility/config.hxx>
#include <workspace/utility/primitives.hxx>
#include <workspace/utility/common.hxx>
#include <workspace/utility/math.hxx>
#include <workspace/utility/logger.hxx>

using namespace External::Common;
using namespace External::Primitives;
using namespace External::Math;

// Memory
//#define is_usermode_mem
#include <impl/memory/kernelmode/include.hxx>
#include <impl/memory/usermode/include.hxx>
#include <impl/memory/memory.hxx>

// Engine / SDK
#include <workspace/engine/offsets.hxx>
#include <workspace/engine/structs.hxx>
#include <workspace/engine/sdk.hxx>

// Core Runtime Cache
#include <workspace/core/runtime/caches/global.hxx>
#include <workspace/core/runtime/caches/registry.hxx>
#include <workspace/core/runtime/caches/engine.hxx>
#include <workspace/core/runtime/caches/camera.hxx>
#include <workspace/core/runtime/caches/entities.hxx>

// Core Gui
#include <workspace/core/features/registry.hxx>
#include <workspace/core/gui/shapes.hxx>
#include <workspace/core/gui/window.hxx>
#include <workspace/core/gui/deps/widgets.hxx>
#include <workspace/core/gui/menu.hxx>
#include <workspace/core/gui/render.hxx>

// Core Features
#include <workspace/core/features/visual/esp.hxx>