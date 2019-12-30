#include<iostream>

#include "fft.h"
#include "audio.h"
#include "screen/screen.h"
#include "basic_shapes/BasicShapes.h"
#include "debug_log/DebugLog.h"

using namespace TEST_SCREEN;

int main(int argc, char* args[])
{
	bool quit = false;

	screen_ptr screen = std::make_shared<Screen>();

	screen->init(800, 600);
	screen->clear();


	for (; quit == false;)
	{
#if 0
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		BasicShapes<>::draw_line(screen, { 0.5, 0.5 }, { x, y }, 0, 1, 0);
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

