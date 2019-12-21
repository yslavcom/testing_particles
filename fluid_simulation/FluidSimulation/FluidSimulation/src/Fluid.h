#pragma once

#include <vector>
#include <memory>

using float_val_array = std::vector<float>;

struct Fluid
{
	int N; // size
	float dt; // time step
	float diff; // diffusion
	float visc; // viscosity

	float_val_array s; // prev density
	float_val_array density;

	float_val_array Vx; // velocity x
	float_val_array Vy; // velocity y
	
	float_val_array Vx0; // prev velocity
	float_val_array Vy0;// prev velocity

	Fluid(int N,
		float dt,
		float diffusion,
		float viscosity)
	{
		this->N = N;
		this->dt = dt;
		this->diff = diffusion;
		this->visc = viscosity;

		size_t array_size = N * N;

		s = float_val_array(array_size);
		density = float_val_array(array_size);
		Vx = float_val_array(array_size);
		Vy = float_val_array(array_size);
		Vx0 = float_val_array(array_size);
		Vy0 = float_val_array(array_size);
	}
};

