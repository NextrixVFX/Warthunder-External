#pragma once

namespace External::Config
{
	// ESP
	static bool g_lines_enabled = true;
	static bool g_boxes_enabled = true; 
	static bool g_usernames_enabled = true;
	static bool g_distance_enabled = true;
	static float g_lines_origin = 1.0f;
	static float g_line_thickness = 1.0f;
	static float g_box_thickness = 1.0f;

    // ESP Colors
    static ImColor g_teammate_color = { 0, 255, 0, 255 };
    static ImColor g_tank_color = { 255, 0, 0, 255 };
    static ImColor g_plane_color = { 128, 0, 255, 255 };

	// Settings
    static bool g_using_efficiency_mode = true;
	static bool g_using_vsync = false;
    static bool g_using_console = true;
	static bool g_is_streamer_mode = true;
	static bool g_debug_menu = false;
	static bool g_is_rendering = false;

	// Menu
	static ImColor g_menu_accent_color{ 109, 127, 140, 255 };
	static float g_menu_accent_darkness = 0.5f;
}

namespace External::Config::Json
{
    inline static rapidjson::Value imcolor_to_json(const ImColor& c, rapidjson::Document::AllocatorType& alloc)
    {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(c.Value.x, alloc);
        arr.PushBack(c.Value.y, alloc);
        arr.PushBack(c.Value.z, alloc);
        arr.PushBack(c.Value.w, alloc);
        return arr;
    }

    inline static ImColor json_to_imcolor(const rapidjson::Value& v)
    {
        if (!v.IsArray() || v.Size() != 4)
            return ImColor{ 255, 255, 255, 255 };

        return ImColor{
            v[0].GetFloat(),
            v[1].GetFloat(),
            v[2].GetFloat(),
            v[3].GetFloat()
        };
    }

    inline static std::string get_config_path()
    {
        const char* appdata = std::getenv(encrypt("APPDATA"));
        if (!appdata)
            return {};

        std::filesystem::path dir = std::filesystem::path(appdata) / encrypt("novision-warthunder");
        std::filesystem::create_directories(dir);

        return (dir / encrypt("config.json")).string();
    }

    inline static bool save_to_json()
    {
        using namespace rapidjson;

        std::string path = get_config_path();
        if (path.empty())
            return false;

        Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();

        // esp
        doc.AddMember("g_lines_enabled", g_lines_enabled, alloc);
        doc.AddMember("g_boxes_enabled", g_boxes_enabled, alloc);
        doc.AddMember("g_usernames_enabled", g_usernames_enabled, alloc);
        doc.AddMember("g_distance_enabled", g_distance_enabled, alloc);
        doc.AddMember("g_lines_origin", g_lines_origin, alloc);
        doc.AddMember("g_line_thickness", g_line_thickness, alloc);
        doc.AddMember("g_box_thickness", g_box_thickness, alloc);

        // colors
        doc.AddMember("g_plane_color", imcolor_to_json(g_plane_color, alloc), alloc);
        doc.AddMember("g_teammate_color", imcolor_to_json(g_teammate_color, alloc), alloc);
        doc.AddMember("g_tank_color", imcolor_to_json(g_tank_color, alloc), alloc);

        // menu colors
        doc.AddMember("g_menu_accent_darkness", g_menu_accent_darkness, alloc);
        doc.AddMember(
            "g_menu_accent_color",
            imcolor_to_json(g_menu_accent_color, alloc),
            alloc
        );

        // misc
        doc.AddMember("g_using_console", g_using_console, alloc);
        doc.AddMember("g_debug_menu", g_debug_menu, alloc);
        doc.AddMember("g_using_efficiency_mode", g_using_efficiency_mode, alloc);
        doc.AddMember("g_is_streamer_mode", g_is_streamer_mode, alloc);
        doc.AddMember("g_using_vsync", g_using_vsync, alloc);
        doc.AddMember("g_is_rendering", g_is_rendering, alloc);

        FILE* fp = fopen(path.c_str(), "wb");
        if (!fp)
            return false;

        thread_local std::array<char, 64 * 1024> buffer;
        FileWriteStream stream(fp, buffer.data(), buffer.size());

        Writer<FileWriteStream> writer(stream);
        doc.Accept(writer);
        fclose(fp);

        return true;
    }

    inline static bool load_from_json()
    {
        using namespace rapidjson;

        std::string path = get_config_path();
        if (path.empty())
            return false;

        FILE* fp = fopen(path.c_str(), "rb");
        if (!fp)
            return false;

        thread_local std::array<char, 64 * 1024> buffer;
        FileReadStream stream(fp, buffer.data(), buffer.size());

        Document doc;
        doc.ParseStream(stream);
        fclose(fp);

        if (doc.HasParseError() || !doc.IsObject())
            return false;

        auto getBool = [&](const char* k, bool& v) {
            if (doc.HasMember(k) && doc[k].IsBool())
                v = doc[k].GetBool();
            };

        auto getInt = [&](const char* k, int& v) {
            if (doc.HasMember(k) && doc[k].IsInt())
                v = doc[k].GetInt();
            };

        auto getFloat = [&](const char* k, float& v) {
            if (doc.HasMember(k) && doc[k].IsNumber())
                v = static_cast<float>(doc[k].GetDouble());
            };

        auto getColor = [&](const char* k, ImColor& v) {
            if (doc.HasMember(k))
                v = json_to_imcolor(doc[k]);
            };


        // esp
        getBool("g_lines_enabled", g_lines_enabled);
        getBool("g_boxes_enabled", g_boxes_enabled);
        getBool("g_usernames_enabled", g_usernames_enabled);
        getBool("g_distance_enabled", g_distance_enabled);
        getFloat("g_lines_origin", g_lines_origin);
        getFloat("g_line_thickness", g_line_thickness);
        getFloat("g_box_thickness", g_box_thickness);

        // colors
        getColor("g_plane_color", g_plane_color);
        getColor("g_teammate_color", g_teammate_color);
        getColor("g_tank_color", g_tank_color);

        // menu colors
        getColor("g_menu_accent_color", g_menu_accent_color);
        getFloat("g_menu_accent_darkness", g_menu_accent_darkness);

        // misc
        getBool("g_using_console", g_using_console);
        getBool("g_debug_menu", g_debug_menu);
        getBool("g_using_efficiency_mode", g_using_efficiency_mode);
        getBool("g_is_streamer_mode", g_is_streamer_mode);
        getBool("g_using_vsync", g_using_vsync);
        getBool("g_is_rendering", g_is_rendering);

        return true;
    }
}