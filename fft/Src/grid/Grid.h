#pragma once

#include "ErrorCode.h"
#include "DebugLog.h"
#include "Screen.h"
#include "BasicShapes.h"
#include "BasicStructs.h"


namespace BASIC_SHAPES_2D
{
	class Grid
	{
	private:
		float m_hor_low_limit;
		float m_hor_hgh_limit;

		float m_vert_low_limit;
		float m_vert_hgh_limit;

		float m_hor_dash_spacing;
		float m_vert_dash_spacing;

		int m_hor_square_count;
		int m_vert_square_count;

		rgb_color_normalized m_hor_grid_color;
		rgb_color_normalized m_vert_grid_color;

	public:
		Grid() :
			m_hor_square_count(10)
			, m_vert_square_count(10)
			, m_hor_low_limit(0.02f)
			, m_hor_hgh_limit(0.98f)
			, m_vert_low_limit(0.02f)
			, m_vert_hgh_limit(0.98f)
			, m_hor_dash_spacing(0.002f)
			, m_vert_dash_spacing(0.002f)
			, m_hor_grid_color(colour_name::WHITE)
			, m_vert_grid_color(colour_name::WHITE)
		{}

		Grid(int hor_square_count, int vert_square_count,
			float hor_low_limit, float hor_hgh_limit,
			float vert_low_limit, float vert_hgh_limit,
			float hor_spacing, float vert_spacing,
			colour_name hor_grid_color, colour_name vert_grid_color
		) :
			m_hor_square_count(hor_square_count)
			, m_vert_square_count(vert_square_count)
			, m_hor_low_limit(hor_low_limit)
			, m_hor_hgh_limit(hor_hgh_limit)
			, m_vert_low_limit(vert_low_limit)
			, m_vert_hgh_limit(vert_hgh_limit)
			, m_hor_dash_spacing(hor_spacing)
			, m_vert_dash_spacing(vert_spacing)
			, m_hor_grid_color(hor_grid_color)
			, m_vert_grid_color(vert_grid_color)
		{}

		template<typename W>
		ErrorCode draw(screen_ptr screen, W&& window, pixel_vec_2d& pixel2d_buf)
		{
			auto hor_lines_distance = (m_vert_hgh_limit - m_vert_low_limit)/ m_vert_square_count;
			for (int index = 0; index < m_vert_square_count; index++)
			{
				auto y = m_vert_low_limit + hor_lines_distance * index;

				BasicShapes::draw_dash_line(screen, 
					std::forward<W>(window),
					pixel2d_buf,
					pixel_2d_coord_normal{ m_hor_low_limit, y },
					pixel_2d_coord_normal{ m_hor_hgh_limit, y },
					m_hor_grid_color,
					m_hor_dash_spacing, m_hor_dash_spacing
				);
			}

			auto ver_lines_distance = (m_hor_hgh_limit - m_hor_low_limit) / m_hor_square_count;
			for (int index = 0; index < m_hor_square_count; index++)
			{
				auto x = m_hor_low_limit + ver_lines_distance * index;

				BasicShapes::draw_dash_line(screen, 
					std::forward<W>(window),
					pixel2d_buf,
					pixel_2d_coord_normal{ x, m_vert_low_limit },
					pixel_2d_coord_normal{ x, m_vert_hgh_limit},
					m_vert_grid_color,
					m_vert_dash_spacing, m_vert_dash_spacing
				);
			}

			return ErrorCode::OK;
		}

		virtual ~Grid() {}


	};
}
