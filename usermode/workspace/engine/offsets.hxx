#pragma once

namespace offsets
{
    // https://dash.monkrel.cc/api/v1/offsets
    // version: 2.55.1.49

    namespace global
    {
        uintptr_t g_game_context = 0x6CB2F68; // uintptr_t<game*>
        uintptr_t g_local_player = 0x6C8F758; // uintptr_t<player*>
    }

    namespace game
    {
        /*
        uintptr_t g_unit_list_1 = 0x310; // uintptr_t
        uintptr_t g_unit_count_1 = 0x320; // uint16_t

        uintptr_t g_unit_list_2 = 0x328; // uintptr_t
        uintptr_t g_unit_count_2 = 0x338; // uint16_t
        */

        uintptr_t g_unit_list_3 = 0x340; // uintptr_t
        uintptr_t g_unit_count_3 = 0x350; // uint16_t

        uintptr_t g_camera_controller = 0x670; // uintptr_t<camera*>
    }

    namespace camera
    {
        uintptr_t g_position = 0x58; // vector3<?>
        uintptr_t g_view_matrix = 0x1D0; // matrix4x4<?>
    }

    namespace player
    {
        uintptr_t m_name = 0x60; // uintptr_t
        //uintptr_t m_owned_unit = 0x8E8; // uintptr_t<unit*>
    }

    namespace unit
    {
        uintptr_t m_unit_type = 0x8C;
        uintptr_t m_bb_min = 0x240;
        uintptr_t m_bb_max = m_bb_min + 0xC;
        uintptr_t m_rotation_matrix = 0xCE4;
        uintptr_t m_position = 0xD08;
        uintptr_t m_player_info = 0xF68;
        uintptr_t m_team_num = 0xFE0;
        uintptr_t m_unit_state = 0xF60;
    }
}