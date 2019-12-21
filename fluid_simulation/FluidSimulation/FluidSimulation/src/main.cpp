#include <iostream>
#include <thread>

#include "Screen.h"
#include "Fluid.h"
#include "ProcessFluid.h"


namespace
{
	const int N = 64;
	const int SCALE = 10;


	void mouseDragged(
		ProcessFluid& processFluid,
		int pmouseX, int pmouseY,
		int mouseX, int mouseY)
	{
		processFluid.addDensity(mouseX / SCALE, mouseY / SCALE, 100);
		float amtX = (mouseX - pmouseX) / SCALE;
		float amtY = (mouseY - pmouseY)/SCALE;
		processFluid.addVelocity(mouseX / SCALE, mouseY / SCALE, amtX, amtY);
	}

	void computeFluid(ProcessFluid& processFluid, bool& quit, int& delta)
	{
		while (!quit)
		{
			if (delta != 0)
			{
				processFluid.step();
			}
		}
	}
}

int main(int argc, char* args[])
{
	std::shared_ptr<Screen> screen = std::make_shared<Screen>();
	screen->init(N * SCALE, N * SCALE);
	screen->clear();

	ProcessFluid processFluid(4,
		std::make_unique<Fluid>(N, 0.1f, 0.0f, 0.0f),
		screen
	);

	bool quit = false;
	bool start_compute = false;
	int pmouseX = 0;
	int pmouseY = 0;

	int delta_t = 0;
	std::thread thread__compute_fluid([&]() {computeFluid(processFluid, quit, delta_t); });

	while(!quit)
	{
		delta_t++;
		//processFluid.step();
		processFluid.renderD(SCALE);

		//draw on screen
		screen->update();

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
				mouseDragged(processFluid, pmouseX, pmouseY, coord.x, coord.y);

				std::cout << "drag mouse with left button down" << coord.x << "    " << coord.y << std::endl;
			}break;
			}
		}
	}

	if (thread__compute_fluid.joinable())
	{
		thread__compute_fluid.join();
	}

	return 0;
}
