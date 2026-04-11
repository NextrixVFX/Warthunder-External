#pragma once

namespace External::Config
{
	// ESP
	inline bool g_lines_enabled = true;
	inline bool g_boxes_enabled = true; 
	inline bool g_usernames_enabled = true;
	inline bool g_distance_enabled = true;
	inline ImColor g_teammate_color = { 0, 255, 0, 255 };
	inline ImColor g_tank_color = { 255, 0, 0, 255 };
	inline ImColor g_plane_color = { 128, 0, 255, 255 };
	inline float g_lines_origin = 1.0f;
	inline float g_line_thickness = 1.0f;
	inline float g_box_thickness = 1.0f;

	// Settings
	inline bool g_vsync_enabled = false;
	inline bool g_capturable_enabled = true;
}