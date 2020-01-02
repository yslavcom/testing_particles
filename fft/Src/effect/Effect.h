#pragma once

#include <algorithm>

#include "ErrorCode.h"
#include "BasicStructs.h"

namespace BASIC_EFFECTS
{
	using namespace BASIC_SHAPES_2D;

	class Effect
	{
	private:
		ErrorCode setPixel(pixel_vec_2d& pixel2d_buf, const pixel_2d_coord&& coord, const rgb_color&& color)
		{
			Uint32 colour = 0;

			auto alpha = 255;

			colour += alpha;
			colour <<= 8;
			colour += color.r;
			colour <<= 8;
			colour += color.g;
			colour <<= 8;
			colour += color.b;
			//colour <<= 8;
			//colour += alpha;

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
		void boxBlur(pixel_vec_2d& pixel2d_buf)
		{
			auto WIDTH = pixel2d_buf.GET_WIDTH();
			auto HEIGHT = pixel2d_buf.GET_HEIGHT();

			for (int y = 0; y < WIDTH; y++)
			{
				for (int x = 0; x < HEIGHT; x++)
				{

					int redTotal = 0;
					int greenTotal = 0;
					int blueTotal = 0;

					for (int row = -1; row <= 1; row++)
					{
						for (int col = -1; col <= 1; col++)
						{
							int currentX = x + col;
							int currentY = y + row;

							if (currentX >= 0 && currentX < WIDTH
								&& currentY >= 0 && currentY < HEIGHT)
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
				}
			}
		}
	};
}
