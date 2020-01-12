#pragma once

#include "BasicShapesDraw.h"
#include "BasicStructs.h"
#include "ErrorCode.h"
#include <vector>


namespace SHAPES_2D
{
	using namespace TEST_SCREEN;

	template<typename Derived>
	class BaseShape
	{
	public:
		ErrorCode draw(pixel_vec_2d& pixel2d_buf)
		{
			return static_cast<Derived*>(this)->draw(pixel2d_buf);
		}

		template<typename W>
		void update_window(W&& window)
		{
			static_cast<Derived*>(this)->update_window(window);
		}
	};

	class Line : BaseShape<Line>
	{
		screen_ptr screen_;
		Screen::ScreenWindow window_;
		pixel_2d_coord_normal start_coord_;
		pixel_2d_coord_normal end_coord_;
		rgb_color_normalized colour_;
		float dash_len_norm_;
		float spacing_len_norm_;

	public:
		Line(
			screen_ptr screen,
			Screen::ScreenWindow window,
			pixel_2d_coord_normal start_coord, pixel_2d_coord_normal end_coord,
			rgb_color_normalized colour,
			float dash_len_norm, float spacing_len_norm)
			: screen_(screen)
			, window_(window)
			, start_coord_(start_coord)
			, end_coord_(end_coord)
			, colour_(colour)
			, dash_len_norm_(dash_len_norm)
			, spacing_len_norm_(spacing_len_norm)
		{}

	public:
		ErrorCode draw(pixel_vec_2d& pixel2d_buf)
		{
			auto rc = BasicShapesDraw::draw_dash_line(screen_,
				std::forward<decltype(window_)>(window_),
				pixel2d_buf,
				std::forward<decltype(start_coord_)>(start_coord_),
				std::forward<decltype(end_coord_)>(end_coord_),
				std::forward<decltype(colour_)>(colour_),
				dash_len_norm_, spacing_len_norm_);
			return rc;
		}

		template<typename W>
		void update_window(W&& window)
		{
			window_ = window;
		}

	};

	template <typename T>
	auto draw(T& base, pixel_vec_2d& pixel2d_buf) {
		return base.draw(pixel2d_buf);
	}

	template <typename T, typename W>
	void update_window(T& base, W&& window)
	{
		base.update_window(window);
	}

	template <typename T>
	void add_to_container(T& base, std::vector<T>& contain)
	{
		contain.emplace_back(base);
	}

}
