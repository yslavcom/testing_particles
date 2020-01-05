#pragma once

#include <algorithm>

#include "ErrorCode.h"
#include "BasicStructs.h"
#include "Screen.h"

namespace BASIC_EFFECTS
{
	using namespace BASIC_SHAPES_2D;

	class Effect
	{
	private:
		ErrorCode setPixel(pixel_vec_2d& pixel2d_buf, const pixel_2d_coord&& coord, const rgb_color&& color)
		{
			auto alpha = 255;
			auto c = std::decay_t<decltype(color)>(color);
			Uint32 colour = to_colour_word(std::forward<decltype(c)>(c), alpha);
			if (!pixel2d_buf(std::forward<decltype(coord)>(coord), colour))
			{
				return ErrorCode::SET_PIXEL_OUT_OF_BOUNDS;
			}
			else
			{
				return ErrorCode::OK;
			}
		}

	public:
		template<typename W>
		void boxBlur(screen_ptr screen, W&& window, pixel_vec_2d& pixel2d_buf)
		{
			auto screenWindow = screen->to_screen_window(window);

			size_t WIDTH_L, WIDTH_H;
			size_t HEIGHT_L, HEIGHT_H;
			auto coord = screenWindow.get_vertex_coord(Screen::ScreenWindow::Vertex::A);
			WIDTH_L = coord.hor;
			HEIGHT_L = coord.ver;

			coord = screenWindow.get_vertex_coord(Screen::ScreenWindow::Vertex::C);
			WIDTH_H = coord.hor;
			HEIGHT_H = coord.ver;

			if (HEIGHT_H < HEIGHT_L)std::swap(HEIGHT_H, HEIGHT_L);
			if (WIDTH_H < WIDTH_L)std::swap(WIDTH_H, WIDTH_L);

			for (int y = HEIGHT_L; y < HEIGHT_H; y++)
			{
				for (int x = WIDTH_L; x < WIDTH_H; x++)
				{
#if 0
					int redTotal = 0;
					int greenTotal = 0;
					int blueTotal = 0;

					for (int row = -1; row <= 1; row++)
					{
						for (int col = -1; col <= 1; col++)
						{
							int currentX = x + col;
							int currentY = y + row;

							if (currentX >= WIDTH_L && currentX < WIDTH_H
								&& currentY >= HEIGHT_L && currentY < HEIGHT_H)
							{
								Uint32 colour = pixel2d_buf({ currentX, currentY });

								Uint8 red = colour >> 24;
								Uint8 green = colour >> 16;
								Uint8 blue = colour >> 8;

								redTotal += red;
								greenTotal += green;
								blueTotal += blue;
							}
						}
					}

					Uint8 red = redTotal / 9;
					Uint8 green = greenTotal / 9;
					Uint8 blue = blueTotal / 9;

					setPixel(pixel2d_buf, { x, y },  {red, green, blue });
#endif
				}
			}
		}
	};
}
