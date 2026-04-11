#pragma once

namespace External::Engine::Structs
{
	struct camera_t
	{
		vector3_t<float> m_positon{};
		matrix4x4_t<float> m_view_matrix{};

		template<typename T = float>
		[[nodiscard]]
		vector2_t<T> project_to_screen(const vector3_t<T>& pos) const noexcept
		{
			// Transform into clip space (homogeneous coordinates)
			const float clip_x =
				pos.x * m_view_matrix.v1.x +
				pos.y * m_view_matrix.v2.x +
				pos.z * m_view_matrix.v3.x +
				m_view_matrix.v4.x;

			const float clip_y =
				pos.x * m_view_matrix.v1.y +
				pos.y * m_view_matrix.v2.y +
				pos.z * m_view_matrix.v3.y +
				m_view_matrix.v4.y;

			const float clip_w =
				pos.x * m_view_matrix.v1.w +
				pos.y * m_view_matrix.v2.w +
				pos.z * m_view_matrix.v3.w +
				m_view_matrix.v4.w;

			// Behind camera check
			if (clip_w < 0.1f)
				return { -1, -1 };

			// Perspective divide → NDC (-1 to 1)
			const float inv_w = 1.0f / clip_w;
			const float ndc_x = clip_x * inv_w;
			const float ndc_y = clip_y * inv_w;

			// Convert NDC → screen space
			vector2_t<T> screen{};
			screen.x = (ndc_x * 0.5f + 0.5f) * static_cast<float>(screen_width);
			screen.y = (0.5f - ndc_y * 0.5f) * static_cast<float>(screen_height);

			return screen;
		}
	};

	struct bbox_t
	{
	private:
		vector3_t<float> transform(vector3_t<float> v, vector3_t<float> pos, matrix3x3_t<float> mat) const
		{
			return { v.x * mat.v1.x + v.y * mat.v2.x + v.z * mat.v3.x + pos.x,
					 v.x * mat.v1.y + v.y * mat.v2.y + v.z * mat.v3.y + pos.y,
					 v.x * mat.v1.z + v.y * mat.v2.z + v.z * mat.v3.z + pos.z };
		}

	public:

		vector3_t<float> m_min{};
		vector3_t<float> m_max{};

		std::array<vector2_t<float>, 8> get_screen_coords(const camera_t& cam, const vector3_t<float>& pos, const matrix3x3_t<float>& rot)
		{
			const std::array<vector3_t<float>, 8> coords =
			{
				vector3_t<float>{m_min.x, m_min.y, m_min.z},
				vector3_t<float>{m_max.x, m_min.y, m_min.z},
				vector3_t<float>{m_max.x, m_max.y, m_min.z},
				vector3_t<float>{m_min.x, m_max.y, m_min.z},
				vector3_t<float>{m_min.x, m_min.y, m_max.z},
				vector3_t<float>{m_max.x, m_min.y, m_max.z},
				vector3_t<float>{m_max.x, m_max.y, m_max.z},
				vector3_t<float>{m_min.x, m_max.y, m_max.z}
			};
			
			std::array<vector2_t<float>, 8> buffer{};

			for (size_t i{0}; i < 8; i++)
			{
				const vector3_t<float>& coord = coords[i];
				const vector3_t<float> transformed = transform(coord, pos, rot);
				buffer[i] = cam.project_to_screen(transformed);
			}

			return buffer;
		}
	};
}