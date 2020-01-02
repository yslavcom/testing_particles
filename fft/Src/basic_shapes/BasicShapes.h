#pragma once

#include <memory>

#include "ErrorCode.h"
#include "screen.h"

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

		inline static ErrorCode set_pixel(pixel_vec_2d& pixel2d_buf, const pixel_2d_coord&& pixel_2d_coord, const rgb_color_normalized&& color, Uint8 alpha)
		{
			Uint32 colour = 0;

			colour += alpha;
			colour <<= 8;
			colour += color.r;
			colour <<= 8;
			colour += color.g;
			colour <<= 8;
			colour += color.b;
			//colour <<= 8;
			//colour += alpha;

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
		static inline ErrorCode draw_dot(const screen_ptr screen, pixel_vec_2d& pixel2d_buf, pixel_2d_coord_normal && coord_normal, rgb_color_normalized&& color, float alpha)
		{
			return set_pixel( pixel2d_buf, 
				screen->convert_to_pixel_2d_coord(std::forward<decltype(coord_normal)>(coord_normal)), 
				std::forward<decltype(color)>(color), 
				alpha);
		}

		/*
		Pass whole size coord and normalized color values to the function
		*/
		static inline ErrorCode draw_dot(const screen_ptr screen, pixel_vec_2d& pixel2d_buf, pixel_2d_coord&& coord, rgb_color_normalized&& color, float alpha)
		{
			return set_pixel(pixel2d_buf, 
				std::forward<decltype(coord)>(coord),
				std::forward<decltype(color)>(color),
				alpha);
		}

		static inline ErrorCode draw_line(screen_ptr screen, pixel_vec_2d& pixel2d_buf,  
			pixel_2d_coord_normal && start, pixel_2d_coord_normal && end, 
			rgb_color_normalized color)
		{
			/*
			http://rosettacode.org/wiki/Xiaolin_Wu%27s_line_algorithm#C.2B.2B

			takes about 50us to draw line in release config
			*/

			auto start_coord_screen = screen->convert_to_pixel_2d_coord(std::forward<decltype(start)>(start));
			auto end_coord_screen = screen->convert_to_pixel_2d_coord(std::forward<decltype(end)>(end));

			auto x0 = start.hor;
			auto y0 = start.ver;
			auto x1 = end.hor;
			auto y1 = end.ver;

			auto ipart = [](float x) -> int {return int(std::floor(x)); };
			auto round = [](float x) -> float {return std::round(x); };
			auto fpart = [](float x) -> float {return x - std::floor(x); };
			auto rfpart = [=](float x) -> float {return 1 - fpart(x); };

			auto plot = [=, &pixel2d_buf , &color](size_t x, size_t y, float brightness) mutable{

				using namespace BASIC_SHAPES_2D;

				auto coord = pixel_2d_coord{ x, y };
				auto r = draw_dot(screen, pixel2d_buf, coord, std::forward<decltype(color)>(color), brightness);

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

			if (steep) {
				for (int x = xpx11 + 1; x < xpx12; x++) {
					plot(ipart(intery), x, rfpart(intery));
					plot(ipart(intery) + 1, x, fpart(intery));
					intery += gradient;
				}
			}
			else {
				for (int x = xpx11 + 1; x < xpx12; x++) {
					plot(x, ipart(intery), rfpart(intery));
					plot(x, ipart(intery) + 1, fpart(intery));
					intery += gradient;
				}
			}

			return ErrorCode::OK;
		}
	};
}
