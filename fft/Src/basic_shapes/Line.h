#pragma once

#include "BasicShapesDraw.h"
#include "BasicStructs.h"
#include "ErrorCode.h"
#include <memory>

namespace BASIC_SHAPES_2D
{
	class Line
	{
		pixel_2d_coord_normal start_coord_;
		pixel_2d_coord_normal end_coord_;
		rgb_color_normalized colour_;
		float dash_len_norm_;
		float spacing_len_norm_;

	public:
		Line(
			pixel_2d_coord_normal start_coord, pixel_2d_coord_normal end_coord,
			rgb_color_normalized colour,
			float dash_len_norm, float spacing_len_norm)
			: start_coord_(start_coord)
			, end_coord_(end_coord)
			, colour_(colour)
			, dash_len_norm_(dash_len_norm)
			, spacing_len_norm_(spacing_len_norm)
		{}

		Line(const Line& other)
			: start_coord_(other.start_coord_)
			, end_coord_(other.end_coord_)
			, colour_(other.colour_)
			, dash_len_norm_(other.dash_len_norm_)
			, spacing_len_norm_(other.spacing_len_norm_)
		{}

	public:
		ErrorCode draw(screen_ptr screen, Screen::ScreenWindow&& window, pixel_vec_2d& pixel2d_buf)
		{
			auto rc = BasicShapesDraw::draw_dash_line(screen,
				std::forward<decltype(window)>(window),
				pixel2d_buf,
				std::forward<decltype(start_coord_)>(start_coord_),
				std::forward<decltype(end_coord_)>(end_coord_),
				std::forward<decltype(colour_)>(colour_),
				dash_len_norm_, spacing_len_norm_);
			return rc;
		}

		ErrorCode draw(screen_ptr screen, const Screen::ScreenWindow& window, pixel_vec_2d& pixel2d_buf)
		{
			auto rc = BasicShapesDraw::draw_dash_line(screen,
				std::forward<decltype(window)>(window),
				pixel2d_buf,
				std::forward<decltype(start_coord_)>(start_coord_),
				std::forward<decltype(end_coord_)>(end_coord_),
				std::forward<decltype(colour_)>(colour_),
				dash_len_norm_, spacing_len_norm_);
			return rc;
		}
	};
}
