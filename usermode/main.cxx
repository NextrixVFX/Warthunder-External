#include <impl/includes.h>

using namespace External;

namespace global
{
	struct window_t
	{
		const std::string m_process_name = encrypt("aces.exe");
		const std::string m_window_name = encrypt("War Thunder");
	} window;
}

int main()
{
	g_memory.initialize(global::window.m_process_name);
	MessageBoxW(
		nullptr,
		encrypt(L"Press OK when loaded in the lobby."),
		encrypt(L"external"),
		MB_OK | MB_ICONINFORMATION
	);

	Render::c_render::initialize(global::window.m_window_name);
	Runtime::register_caches();
	Runtime::start_caches();
	Features::register_features();
	
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