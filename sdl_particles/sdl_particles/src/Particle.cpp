#include "Particle.h"

#include <math.h>
#include <stdlib.h>

namespace caveofprogramming
{
	Particle::Particle():m_x(0), m_y(0)
	{
		init();
	}

	Particle::~Particle()
	{

	}

	void Particle::init()
	{
		m_x = 0;
		m_y = 0;

		m_direction = (2 * pi_value * rand()) / RAND_MAX;
		m_speed = (0.04 * rand()) / RAND_MAX;

		m_speed *= m_speed;
	}

	void Particle::update(int interval)
	{
		m_direction += interval * 0.0002;

		double xspeed = m_speed * cos(m_direction);
		double yspeed = m_speed * sin(m_direction);

		m_x += xspeed * interval;
		m_y += yspeed * interval;

		if (m_x < -1 || m_x > 1
			|| m_y < -1 || m_y > 1)
		{
			init();
		}

		if (rand() < RAND_MAX / 100)
		{
			init();
		}
	}
}