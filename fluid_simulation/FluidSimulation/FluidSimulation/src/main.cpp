#include <iostream>
#include <thread>
#include <chrono>

#include "Screen.h"
#include "Fluid.h"
#include "ProcessFluid.h"

/*
https://www.youtube.com/watch?v=alhpH6ECFvQ
https://www.mikeash.com/pyblog/fluid-simulation-for-dummies.html
*/

namespace
{
	const int N = 64;
	const int SCALE = 5;

	void mouseDragged(
		ProcessFluid& processFluid,
		int pmouseX, int pmouseY,
		int mouseX, int mouseY, int time_step)
	{
		processFluid.addDensity(mouseX / SCALE, mouseY / SCALE, time_step);
		float amtX = (mouseX - pmouseX) / SCALE;
		float amtY = (mouseY - pmouseY)/SCALE;
		processFluid.addVelocity(mouseX / SCALE, mouseY / SCALE, amtX, amtY);
	}

	void computeFluid(ProcessFluid& processFluid, bool& quit)
	{
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		processFluid.step();
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "dt = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[ µs ]" << std::endl;
	}

	void drawFluid(ProcessFluid& processFluid, std::shared_ptr<Screen> screen)
	{
		processFluid.renderD(SCALE);
		processFluid.fadeD();

		//draw on screen
		screen->update();
	}
}

int main(int argc, char* args[])
{

	std::shared_ptr<Screen> screen = std::make_shared<Screen>();
	screen->init(N * SCALE, N * SCALE);
	screen->clear();

	int time_step = 100;//100milliseconds
	//int frame_rate = 30;//300 ms

	int iterations = 8;
	ProcessFluid processFluid(iterations,
		//std::make_unique<Fluid>(N, 0.1f, 0.0f, 0.0f),
		std::make_unique<Fluid>(N, static_cast<float>(time_step)/1000.0, 0.0, 0.0005),
		screen
	);


	bool quit = false;
	bool start_compute = false;
	int pmouseX = 0;
	int pmouseY = 0;

	std::thread fluid_compute_thread(
		[time_step, &processFluid, &quit]() {
			while (!quit)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(time_step));

				computeFluid(processFluid, quit);
			}

			if (quit)
			{
				std::cout << "quit 'computeFluid'" << std::endl;
			}
		}
	);


#if 0
	std::thread draw_fluid_thread(
		[frame_rate, &processFluid, screen, &quit]()
		{
			while (!quit)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(frame_rate));
				drawFluid(processFluid, screen);
			}
			if (quit)
			{
				std::cout << "quit 'drawFluid'" << std::endl;
			}
		});
#endif

	while(!quit)
	{
		drawFluid(processFluid, screen);

		//process events
		auto is_event = screen->processEvents();
		if (is_event.has_value())
		{
			switch (is_event.value().first)
			{
			case Screen::EventType::Quit:
			{
				std::cout << "quitting" << std::endl;

				quit = true;
			}break;

			case Screen::EventType::LeftMouseDown:
			{
				auto coord = std::get<Screen::coord>(is_event.value().second);
				pmouseX = coord.x;
				pmouseY = coord.x;

				std::cout << "left mouse button down " << pmouseX << "    " << pmouseY << std::endl;
			}break;

			case Screen::EventType::MouseDragging:
			{
				auto coord = std::get<Screen::coord>(is_event.value().second);
				mouseDragged(processFluid, pmouseX, pmouseY, coord.x, coord.y, time_step);

				std::cout << "drag mouse with left button down" << coord.x << "    " << coord.y << std::endl;
			}break;
			}
		}
	}

	if (fluid_compute_thread.joinable())
	{
		fluid_compute_thread.join();
	}

#if 0
	if (draw_fluid_thread.joinable())
	{
		draw_fluid_thread.join();
	}
#endif
	return 0;
}
