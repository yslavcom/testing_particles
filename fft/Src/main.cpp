#include<iostream>

#include "fft.h"
#include "audio.h"
#include "screen.h"
#include "BasicShapes.h"
#include "DebugLog.h"
#include "Effect.h"
#include "Grid.h"
#include "Axis.h"

using namespace TEST_SCREEN;
using namespace BASIC_SHAPES_2D;

int main(int argc, char* args[])
{
	bool quit = false;

	screen_ptr screen = std::make_shared<Screen>();

	screen->init(1024, 768);
	screen->clear();

	Grid grid;
	Axis axis_x(Axis::Type::X, {colour_name::WHITE});
	Axis axis_y(Axis::Type::Y, { colour_name::WHITE });

#if 1
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	pixel_vec_2d pixel2d_buf(screen->SCREEN_WIDTH, screen->SCREEN_HEIGHT);

	Screen::Window window{ {0.7f, 0.5f}, 0.19f, 0.19f };

	grid.draw(screen, Screen::Window{ {0.1f, 0.1f}, 0.5f, 0.5f }, pixel2d_buf);

	grid.draw(screen, window, pixel2d_buf);
	BasicShapes::draw_dash_line(screen, window, pixel2d_buf, pixel_2d_coord_normal{ 0.5, 0.1 }, pixel_2d_coord_normal{ 0.5, 0.9 }, rgb_color_normalized{ colour_name::RED }, 0.1f, 0.01f);
	BasicShapes::draw_dash_line(screen, window, pixel2d_buf, pixel_2d_coord_normal{ 0.1, 0.1 }, pixel_2d_coord_normal{ 0.9, 0.9 }, rgb_color_normalized{ colour_name::RED }, 0.1f, 0.01f);
	BasicShapes::draw_dash_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.1, 0.1 }, pixel_2d_coord_normal{ 0.9, 0.9 }, rgb_color_normalized{ colour_name::BLUE }, 0.1f, 0.01f);
#if 0
	BasicShapes::draw_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.4, 0.4 }, pixel_2d_coord_normal{ 0.7, 0.4 }, rgb_color_normalized{ colour_name::GREEN });
	BasicShapes::draw_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.2, 0.5 }, pixel_2d_coord_normal{ 0.8, 0.5 }, rgb_color_normalized{ colour_name::BLUE });
	BasicShapes::draw_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.5, 0.2 }, pixel_2d_coord_normal{ 0.5, 0.4 }, rgb_color_normalized{ colour_name::WHITE });
	BasicShapes::draw_dot(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.35, 0.35 }, rgb_color_normalized{ 0.5, 0.5, 1 }, 255);
	BasicShapes::draw_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.11, 0.2 }, pixel_2d_coord_normal{ 0.6, 0.95 }, rgb_color_normalized{ 0.1, 0.4, 0.4 });
	BasicShapes::draw_dash_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.11, 0.2 }, pixel_2d_coord_normal{ 0.91, 0.61}, rgb_color_normalized{ colour_name::WHITE }, 0.1f, 0.1f);
	BasicShapes::draw_dash_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.1, 0.1 }, pixel_2d_coord_normal{ 0.9, 0.9 }, rgb_color_normalized{ colour_name::GREEN }, 0.25f, 0.25f);
	BasicShapes::draw_dash_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.1, 0.1 }, pixel_2d_coord_normal{ 0.9, 0.1 }, rgb_color_normalized{ colour_name::GREEN }, 0.25f, 0.25f);
	BasicShapes::draw_dash_line(screen, Screen::Window{}, pixel2d_buf, pixel_2d_coord_normal{ 0.1, 0.1 }, pixel_2d_coord_normal{ 0.1, 0.9 }, rgb_color_normalized{ colour_name::GREEN }, 0.1f, 0.01f);
#endif
	axis_x.draw(screen, Screen::Window{}, pixel2d_buf);
	axis_y.draw(screen, Screen::Window{}, pixel2d_buf);

	screen->copy_to_screen_buf(pixel2d_buf);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	DebugLog::instance()->print("dt = " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + "[ us ]");
#endif

	for (; quit == false;)
	{
		{
			/*
			the screen functions take about 16 ms in release config,
			it looks like it frame freq depended (?)
			*/
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
			screen->clear_render();
			screen->update_from_pixel_buffer();
			screen->present_render();
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			//DebugLog::instance()->print("dt = " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + "[ us ]");
		}


		auto is_event = screen->processEvents();
		if (is_event.has_value())
		{
			switch (is_event.value().first)
			{
			case Screen::EventType::Quit:
					quit = true;
					DebugLog::instance()->print("quitting...");
					break;
			}
		}
	}

	return 0;
}

