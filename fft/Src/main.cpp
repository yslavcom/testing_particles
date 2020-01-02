#include<iostream>

#include "fft.h"
#include "audio.h"
#include "screen.h"
#include "BasicShapes.h"
#include "DebugLog.h"
#include "Grid.h"
#include "Effect.h"

using namespace TEST_SCREEN;
using namespace BASIC_SHAPES_2D;

int main(int argc, char* args[])
{
	bool quit = false;

	screen_ptr screen = std::make_shared<Screen>();

	screen->init(800, 600);
	screen->clear();

	Grid grid;

	for (; quit == false;)
	{
#if 1
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		pixel_vec_2d pixel_vec_2d(screen->SCREEN_WIDTH, screen->SCREEN_HEIGHT);
		BasicShapes::draw_line(screen, pixel_vec_2d, { 0.4, 0.4 }, { 0.4, 0.7 }, { colour_name::RED });
		BasicShapes::draw_line(screen, pixel_vec_2d, { 0.4, 0.4 }, { 0.7, 0.4 }, { colour_name::GREEN });
		BasicShapes::draw_line(screen, pixel_vec_2d, { 0.2, 0.5 }, { 0.8, 0.5 }, { colour_name::BLUE });
		BasicShapes::draw_line(screen, pixel_vec_2d, { 0.5, 0.2 }, { 0.5, 0.4 }, { colour_name::WHITE });
		BasicShapes::draw_dot(screen, pixel_vec_2d, { 0.35, 0.35 }, { 0.5, 0.5, 1}, 255);
		screen->copy_to_screen_buf(pixel_vec_2d);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//	DebugLog::instance()->print("dt = " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + "[ us ]");
#endif

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

