#pragma once

namespace offsets
{
    // https://dash.monkrel.cc/api/v1/offsets
    // version: 2.55.1.80

    inline std::unordered_map<std::string, uintptr_t> values{};

    bool load(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (line.starts_with("--"))
                continue;

            std::istringstream iss(line);
            std::string name;
            uintptr_t value;

            if (!(iss >> name >> std::hex >> value))
                continue;

            External::Logger::print(encrypt("Offset: %s: 0x%llX"), name.c_str(), value);
            values[name] = value;
        }

        return true;
    }

    uintptr_t get(const std::string& name)
    {
        auto it = values.find(name);
        if (it == values.end())
            return 0;

        return it->second;
    }

    namespace global
    {
        inline uintptr_t& g_game_context = values["global.g_game_context"];
        inline uintptr_t& g_local_player = values["global.g_local_player"];
    }

    namespace game
    {
        inline uintptr_t& g_unit_list_3 = values["game.g_unit_list_3"];
        inline uintptr_t& g_unit_count_3 = values["game.g_unit_count_3"];
        inline uintptr_t& g_camera_controller = values["game.g_camera_controller"];
    }

    namespace camera
    {
        inline uintptr_t& g_position = values["camera.g_position"];
        inline uintptr_t& g_view_matrix = values["camera.g_view_matrix"];
    }

    namespace player
    {
        inline uintptr_t& m_name = values["player.m_name"];
    }

    namespace unit
    {
        inline uintptr_t& m_unit_type = values["unit.m_unit_type"];
        inline uintptr_t& m_bb_min = values["unit.m_bb_min"];
        inline uintptr_t& m_bb_max = values["unit.m_bb_max"];
        inline uintptr_t& m_rotation_matrix = values["unit.m_rotation_matrix"];
        inline uintptr_t& m_position = values["unit.m_position"];
        inline uintptr_t& m_team_num = values["unit.m_team_num"];
        inline uintptr_t& m_unit_state = values["unit.m_unit_state"];
        inline uintptr_t& m_player_info = values["unit.m_player_info"];
    }
    
}
