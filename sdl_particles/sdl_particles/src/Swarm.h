#pragma once

#include "Particle.h"
#include <memory>
#include <array>

namespace caveofprogramming
{
	class Swarm
	{
	public:
		const static int NPARTICLES = 20000;

	private:
		mutable std::array<std::shared_ptr<Particle>, NPARTICLES> m_pParticles;
		int lastTime;

	public:
		Swarm();
		virtual ~Swarm();
		void update(int elapsed);

		auto& getParticles() const {
			return m_pParticles;
		};

	};
}