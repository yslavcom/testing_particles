#pragma once

#include "Screen.h"
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

		float m_hor_spacing;
		float m_vert_spacing;

		int m_hor_square_count;
		int m_vert_square_count;

		rgb_color_normalized m_hor_grid_color;
		rgb_color_normalized m_vert_grid_color;

	public:
		Grid() :
			m_hor_square_count(10)
			, m_vert_square_count(10)
			, m_hor_low_limit(0.02)
			, m_hor_hgh_limit(0.98)
			, m_vert_low_limit(0.02)
			, m_vert_hgh_limit(0.98)
			, m_hor_spacing(0.02)
			, m_vert_spacing(0.98)
			, m_hor_grid_color(colour_name::WHITE)
			, m_vert_grid_color(colour_name::WHITE)
		{}

		Grid(int hor_square_count, int vert_square_count,
			int hor_low_limit, int hor_hgh_limit,
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
			, m_hor_spacing(hor_spacing)
			, m_vert_spacing(vert_spacing)
			, m_hor_grid_color(hor_grid_color)
			, m_vert_grid_color(vert_grid_color)
		{}

		ErrorCode draw(screen_ptr screen, pixel_vec_2d& pixel2d_buf)
		{


			return ErrorCode::OK;
		}

		virtual ~Grid() {}


	};
}
