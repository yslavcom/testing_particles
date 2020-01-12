#pragma once

#include<vector>
#include "Screen.h"
#include "ErrorCode.h"
#include "BasicShapesDraw.h"

namespace BASIC_SHAPES_2D
{
	using namespace TEST_SCREEN;

	class Widget
	{
	private:
		screen_ptr screen;
		Screen::ScreenWindow window;

		std::vector<> shapes;

	public:
		Widget(const Screen::ScreenWindow& win, screen_ptr screen);
		Widget(Screen::ScreenWindow&& win, screen_ptr screen);

		~virtual Widget();
		Widget(const Widget& other);
		Widget(Widget&& other);
		Widget operator=(const Widget& other);
		Widget operator=(Widget&& other);

		ErrorCode add_shape();
		ErrorCode draw();
		ErrorCode move();

	};
}
