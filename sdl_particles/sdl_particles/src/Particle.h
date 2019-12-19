#pragma once

namespace caveofprogramming
{
	class Particle
	{
	private:
		const double pi_value = 3.1415926535897932384626433832795;

		void init();
		double m_speed;
		double m_direction;

	public:
		double m_x, m_y;
		Particle();
		virtual ~Particle();
		void update(int interval);
	};
}