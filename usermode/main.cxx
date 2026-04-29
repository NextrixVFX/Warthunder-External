#include <impl/includes.h>

using namespace External;

namespace global
{
	struct window_t
	{
		const std::string m_process_name = encrypt("aces.exe");
		const std::string m_window_name_1 = encrypt("War Thunder");
		const std::string m_window_name_2 = encrypt("War Thunder - In battle");
	} g_window;
}

int main()
{
	if (!g_memory.initialize(global::g_window.m_process_name))
	{
		MessageBoxW(
			nullptr,
			encrypt(L"Failed to initialize driver. Try restarting ur pc"),
			encrypt(L"Error"),
			MB_OK | MB_ICONINFORMATION
		);
		return 1;
	}

	if (!offsets::load(encrypt("assets\\offsets.txt")))
	{
		MessageBoxW(
			nullptr,
			encrypt(L"Failed to load assets\\offsets.txt"),
			encrypt(L"Error"),
			MB_OK | MB_ICONINFORMATION
		);
		return 1;
	}

	MessageBoxW(
		nullptr,
		encrypt(L"Press OK when loaded in the lobby."),
		encrypt(L"external"),
		MB_OK | MB_ICONINFORMATION
	);
	
	Runtime::register_caches();
	Runtime::start_caches();
	Features::register_features();
	Render::c_render::initialize(global::g_window.m_window_name_1, global::g_window.m_window_name_2);
	
	static bool running = true;
	while (running)
	{
		if (GetAsyncKeyState(VK_F12))
			running = false;

		Features::on_update();
		sleep_short(1);
	}

	Runtime::stop_caches();
	Render::c_render::destroy_device();

	std::cin.get();
	return 0;
}