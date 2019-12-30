#pragma once

#include <memory>

#include "ErrorCode.h"
#include "../screen/screen.h"

using namespace TEST_SCREEN;

template<typename T=float>
class BasicShapes
{
public:
	struct coord
	{
		T x;
		T y;
	};

private:
	BasicShapes() = delete;
	BasicShapes(const BasicShapes&) = delete;
	BasicShapes(const BasicShapes&&) = delete;
	BasicShapes operator=(const BasicShapes&) = delete;
	BasicShapes operator=(const BasicShapes&&) = delete;

public:
	/*
	Pass only normalized values to the function
	*/
	static inline ErrorCode draw_dot(screen_ptr screen, const coord&& coord, float alpha, float red, float green, float blue)
	{
		return screen->setPixel({ coord.x , coord.y },
			alpha * Screen::MAX_ALPHA_VALUE,
			{ red , green , blue }
		);
	}

	static inline ErrorCode draw_line(screen_ptr screen, const coord&& start, const coord&& end, float red, float green, float blue)
	{
		/*
		http://rosettacode.org/wiki/Xiaolin_Wu%27s_line_algorithm#C.2B.2B

		takes about 50us to draw line in release config
		*/

		auto x0 = start.x * Screen::SCREEN_WIDTH;
		auto y0 = start.y * Screen::SCREEN_HEIGHT;
		auto x1 = end.x * Screen::SCREEN_WIDTH;
		auto y1 = end.y * Screen::SCREEN_HEIGHT;

		auto ipart = [](float x) -> int {return int(std::floor(x)); };
		auto round = [](float x) -> float {return std::round(x); };
		auto fpart = [](float x) -> float {return x - std::floor(x); };
		auto rfpart = [=](float x) -> float {return 1 - fpart(x); };

		auto plot = [=](int x, int y, float brightness) {
			auto r = draw_dot(screen, { static_cast<T>(x)/Screen::SCREEN_WIDTH, static_cast<T>(y) / Screen::SCREEN_HEIGHT }, brightness, red, green, blue);

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
