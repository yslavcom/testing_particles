//#include <algorithm>

#include "Swarm.h"



namespace caveofprogramming
{
	Swarm::Swarm():lastTime(0)
	{
		for (auto& el : m_pParticles)
		{
			el = std::make_shared<Particle>();
		}
	}

	Swarm::~Swarm()
	{}

	void Swarm::update(int elapsed)
	{
		int interval = elapsed - lastTime;

		for (int i = 0; i < Swarm::NPARTICLES; i++)
		{
			m_pParticles[i]->update(interval);
		}

		lastTime = elapsed;
	}
}