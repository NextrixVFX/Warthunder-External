#include <impl/includes.hxx>

using namespace External;

namespace global
{
	static bool g_is_running = true;

	struct window_t
	{
		const std::string m_process_name = encrypt("aces.exe");
		const std::string m_window_name = encrypt("War Thunder");
	} g_window;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	Common::toggle_console(true);
	SetConsoleTitleA(encrypt("initializing..."));

	SetConsoleTitleA(encrypt("loading offsets..."));
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

	SetConsoleTitleA(encrypt("waiting for game..."));
	Logger::print(encrypt("[c_main] winmain: Waiting for game process..."));

	if (!g_memory.initialize(global::g_window.m_process_name))
	{
		Logger::error(encrypt("[c_main] winmain: Failed to initialize memory module! Relaunch; if the issue presists, restart your pc."));
		return std::cin.get();
	}

	SetConsoleTitleA(encrypt("novision.cc"));
	Config::Json::load_from_json();
	MessageBoxW(
		nullptr,
		encrypt(L"Press OK when loaded in the lobby."),
		encrypt(L"novision.cc"),
		MB_OK | MB_ICONINFORMATION
	);

	Runtime::register_caches();
	Runtime::start_caches();
	Features::register_features();
	Render::g_render.initialize(global::g_window.m_window_name);

	static std::string random_str = random_string(12);
	auto cleanup = []() -> void
	{
		global::g_is_running = false;
		std::cout << std::endl;
		Config::Json::save_to_json();
		Features::stop_features();
		Runtime::stop_caches();
		Render::g_render.destroy_device();
	};

	while (global::g_is_running)
	{
		sleep_short(Config::g_using_efficiency_mode ? 8 : 2);
		Features::on_update();

		Common::toggle_console(Config::g_using_console);
		SetConsoleTitleA((Config::g_is_streamer_mode) ? encrypt("Edge") : random_str.c_str());

		if (GetAsyncKeyState(VK_F10))
			cleanup();
	}

	return 0;
}