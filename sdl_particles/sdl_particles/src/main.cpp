#include <math.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>

#include "Screen.h"
#include "Swarm.h"

using namespace caveofprogramming;

int main(int argc, char* args[])
{
	srand(time(NULL));	

	Screen screen;
	if (!screen.init())
	{
		std::cout << "Error initializing SDL " << std::endl;
	}

	Swarm swarm;

	while (true)
	{
		//update particles
		//draw particles

		int elapsed = SDL_GetTicks();

		swarm.update(elapsed);

		Uint8 red = (1 + sin(elapsed * 0.0001)) * 128;
		Uint8 green = (1 + sin(elapsed * 0.0002)) * 128;
		Uint8 blue = (1 + sin(elapsed * 0.0003)) * 128;

		auto rParticles = swarm.getParticles();
		for (int i = 0; i < Swarm::NPARTICLES; i++)
		{
			auto particle = rParticles[i];
			int x = (particle->m_x + 1) * Screen::SCREEN_WIDTH / 2;
			int y = (particle->m_y  * Screen::SCREEN_WIDTH / 2) + Screen::SCREEN_HEIGHT/2;

			screen.setPixel(x, y, red, green, blue);
			//screen.setPixel(x, y, 255, 255, 255);
		}

		screen.boxBlur();
	
		//draw the screen
		screen.update();

		//check for messages/events
		if (!screen.processEvents()) {
			break;
		}
	}

	screen.close();

	return 0;
}