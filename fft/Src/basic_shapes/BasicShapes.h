#pragma once

#include <memory>

#include "ErrorCode.h"
#include "screen.h"
#include "BasicStructs.h"

namespace BASIC_SHAPES_2D
{
	using namespace TEST_SCREEN;

	class BasicShapes
	{
	private:
		BasicShapes() = delete;
		BasicShapes(const BasicShapes&) = delete;
		BasicShapes(const BasicShapes&&) = delete;
		BasicShapes operator=(const BasicShapes&) = delete;
		BasicShapes operator=(const BasicShapes&&) = delete;

		inline static ErrorCode set_pixel(pixel_vec_2d& pixel2d_buf, pixel_2d_coord&& pixel_2d_coord, rgb_color_normalized&& color, Uint8 alpha)
		{
			auto c = std::decay_t<decltype(color)>(color);
			Uint32 colour = to_colour_word(std::forward<decltype(c)>(c), alpha);
			if (!pixel2d_buf(std::forward<decltype(pixel_2d_coord)>(pixel_2d_coord), colour))
			{
				return ErrorCode::SET_PIXEL_OUT_OF_BOUNDS;;
			}
			else
			{
				return ErrorCode::OK;
			}
		}

	public:
		/*
		Pass only normalized values for the coord and color to the function
		*/
		static inline ErrorCode draw_dot(const screen_ptr screen, pixel_vec_2d& pixel2d_buf, pixel_2d_coord_normal&& coord_normal, rgb_color_normalized&& color, float alpha)
		{
			return set_pixel(pixel2d_buf,
				screen->convert_to_pixel_2d_coord(std::forward<decltype(coord_normal)>(coord_normal)),
				std::forward<decltype(color)>(color),
				alpha);
		}

		/*
		Pass whole size coord and normalized color values to the function
		*/
		static inline ErrorCode draw_dot_screen(const screen_ptr screen, pixel_vec_2d& pixel2d_buf, pixel_2d_coord&& coord, rgb_color_normalized&& color, float alpha)
		{
			return set_pixel(pixel2d_buf,
				std::forward<decltype(coord)>(coord),
				std::forward<decltype(color)>(color),
				alpha);
		}

		static inline ErrorCode draw_dash_line(screen_ptr screen,
			pixel_vec_2d& pixel2d_buf,
			pixel_2d_coord_normal&& start, pixel_2d_coord_normal&& end,
			rgb_color_normalized&& color,
			const float dash_len_norm, const float spacing_len_norm
		)
		{
			/*
			http://rosettacode.org/wiki/Xiaolin_Wu%27s_line_algorithm#C.2B.2B

			takes about 50us to draw line in release config
			*/

			auto start_coord_screen = screen->convert_to_pixel_2d_coord(std::forward<decltype(start)>(start));
			auto end_coord_screen = screen->convert_to_pixel_2d_coord(std::forward<decltype(end)>(end));

			int x0 = start_coord_screen.hor;
			int y0 = start_coord_screen.ver;
			int x1 = end_coord_screen.hor;
			int y1 = end_coord_screen.ver;

			auto ipart = [](float x) -> int {return int(std::floor(x)); };
			auto round = [](float x) -> float {return std::round(x); };
			auto fpart = [](float x) -> float {return x - std::floor(x); };
			auto rfpart = [=](float x) -> float {return 1 - fpart(x); };

			auto plot = [=, &pixel2d_buf, &color](size_t x, size_t y, float brightness) {

				auto coord = pixel_2d_coord{ x, y };
				auto r = set_pixel(pixel2d_buf,
					std::forward<decltype(coord)>(coord),
					std::forward<rgb_color_normalized>(color),
					brightness);

				//DebugLog::instance()->print("brightness = " + std::to_string(brightness));

				if (r != ErrorCode::OK)
				{
					DebugLog::instance()->print("draw_line::plot fail = " + std::to_string(x) + " " + std::to_string(y));
				}
			};

			const bool steep = abs(y1 - y0) > abs(x1 - x0);
			if (steep) {
				std::swap(x0, y0);
				std::swap(x1, y1);
			}
			if (x0 > x1) {
				std::swap(x0, x1);
				std::swap(y0, y1);
			}

			const float dx = x1 - x0;
			const float dy = y1 - y0;
			const float gradient = (dx == 0) ? 1 : dy / dx;

			size_t dash_len = 0;
			size_t spacing_len = 0;
			const bool is_dash_line = (0 != spacing_len_norm);
			if (is_dash_line)
			{
				dash_len = screen->convert_normalized_len_to_screen_len(dash_len_norm, gradient);
				spacing_len = screen->convert_normalized_len_to_screen_len(spacing_len_norm, gradient);
			}

			int xpx11;
			float intery;
			{
				const float xend = round(x0);
				const float yend = y0 + gradient * (xend - x0);
				const float xgap = rfpart(x0 + 0.5);
				xpx11 = int(xend);
				const int ypx11 = ipart(yend);
				if (steep) {
					plot(ypx11, xpx11, rfpart(yend) * xgap);
					plot(ypx11 + 1, xpx11, fpart(yend) * xgap);
				}
				else {
					plot(xpx11, ypx11, rfpart(yend) * xgap);
					plot(xpx11, ypx11 + 1, fpart(yend) * xgap);
				}
				intery = yend + gradient;
			}

			int xpx12;
			{
				const float xend = round(x1);
				const float yend = y1 + gradient * (xend - x1);
				const float xgap = rfpart(x1 + 0.5);
				xpx12 = int(xend);
				const int ypx12 = ipart(yend);
				if (steep) {
					plot(ypx12, xpx12, rfpart(yend) * xgap);
					plot(ypx12 + 1, xpx12, fpart(yend) * xgap);
				}
				else {
					plot(xpx12, ypx12, rfpart(yend) * xgap);
					plot(xpx12, ypx12 + 1, fpart(yend) * xgap);
				}
			}

			size_t dash_iter = 0;
			size_t spacing_iter = 0;
			bool insert_spacing = false;
			auto process_dash = [&]()
			{
				if (!insert_spacing)dash_iter++;
				else if (insert_spacing)spacing_iter++;
				if (spacing_len <= spacing_iter)
				{
					dash_iter = 0;
					spacing_iter = 0;
				}
			};

			if (steep) {
				for (int x = xpx11 + 1; x < xpx12; x++) {

					insert_spacing = is_dash_line && (dash_len <= dash_iter);
					if (!insert_spacing)
					{
						plot(ipart(intery), x, rfpart(intery));
						plot(ipart(intery) + 1, x, fpart(intery));
					}
					intery += gradient;

					if (is_dash_line)
					{
						process_dash();
					}
				}
			}
			else {
				for (int x = xpx11 + 1; x < xpx12; x++) {

					insert_spacing = is_dash_line && (dash_len <= dash_iter);
					if (!insert_spacing)
					{
						plot(x, ipart(intery), rfpart(intery));
						plot(x, ipart(intery) + 1, fpart(intery));
					}
					intery += gradient;

					if (is_dash_line)
					{
						process_dash();
					}
				}
			}

			return ErrorCode::OK;
		}


		static inline ErrorCode draw_line(screen_ptr screen,
			pixel_vec_2d& pixel2d_buf,
			pixel_2d_coord_normal&& start, pixel_2d_coord_normal&& end,
			rgb_color_normalized&& color)
		{
			return draw_dash_line(screen,
				pixel2d_buf,
				std::forward<pixel_2d_coord_normal>(start), std::forward<pixel_2d_coord_normal>(end),
				std::forward<rgb_color_normalized>(color),
				1, 0);
		}


	};
}
