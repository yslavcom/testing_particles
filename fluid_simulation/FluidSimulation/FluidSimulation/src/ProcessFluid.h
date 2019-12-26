#pragma once

#include <memory>
#include <mutex>
#include <algorithm>

#include "Fluid.h"
#include "Screen.h"

class ProcessFluid
{
private:
	int iter;
	int N;

	std::mutex mutex;

	std::unique_ptr<Fluid> fluid;
	std::shared_ptr<Screen> screen;

public:

	ProcessFluid(int iter,
		std::unique_ptr<Fluid> fluid,
		std::shared_ptr<Screen> screen )
	{
		this->iter = iter;

		this->fluid = std::move(fluid);
		this->N = this->fluid->N;

		this->screen = screen;
	}

private:
	void lin_solve(int b, float_val_array& x, float_val_array& x0, float a, float c)
	{
		float cRecip = 1.0 / c;

		for (int j = 1; j < N - 1; j++) 
		{
			for (int i = 1; i < N - 1; i++) 
			{
				x[IX(i, j)] =
					(x0[IX(i, j)]
						+ a * (x[IX(i + 1, j)]
							+ x[IX(i - 1, j)]
							+ x[IX(i, j + 1)]
							+ x[IX(i, j - 1)]
							)) * cRecip;
			}
		}
		set_bnd(b, x);
	}

	void set_bnd(int b, float_val_array& x)
	{
		for (int i = 1; i < N - 1; i++) {
			x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
			x[IX(i, N - 1)] = b == 2 ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
		}

		for (int j = 1; j < N - 1; j++) {
			x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
			x[IX(N - 1, j)] = b == 1 ? -x[IX(N - 2, j)] : x[IX(N - 2, j)];
		}

		x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
		x[IX(0, N - 1)] = 0.5f * (x[IX(1, N - 1)] + x[IX(0, N - 2)]);
		x[IX(N - 1, 0)] = 0.5f * (x[IX(N - 2, 0)] + x[IX(N - 1, 1)]);
		x[IX(N - 1, N - 1)] = 0.5f * (x[IX(N - 2, N - 1)] + x[IX(N - 1, N - 2)]);
	}

	int IX(int x, int y)
	{
		if (x < 0)x = 0;
		else if (x >= N)x = N-1;

		if (y < 0)y = 0;
		else if (y >= N)y = N - 1;

		return x + y * N;
	}


	void diffuse(int b, float_val_array& x, float_val_array& x0, float diff, float dt)
	{
		float a = dt * diff * (N - 2) * (N - 2);
		lin_solve(b, x, x0, a, 1 + 6 * a);
	}

	void project(float_val_array& velocX, float_val_array& velocY, float_val_array& p, float_val_array& div)
	{
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				div[IX(i, j)] = -0.5f * (
					velocX[IX(i + 1, j)]
					- velocX[IX(i - 1, j)]
					+ velocY[IX(i, j + 1)]
					- velocY[IX(i, j - 1)]
					) / N;
				p[IX(i, j)] = 0;
			}
		}
		set_bnd(0, div);
		set_bnd(0, p);
		lin_solve(0, p, div, 1, 6);

		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)]
					- p[IX(i - 1, j)]) * N;
				velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)]
					- p[IX(i, j - 1)]) * N;
			}
		}
		set_bnd(1, velocX);
		set_bnd(2, velocY);
	}

	void advect(int b, float_val_array& d, float_val_array& d0, float_val_array& velocX, float_val_array& velocY, float dt)
	{
		float i0, i1, j0, j1;

		float dtx = dt * (N - 2);
		float dty = dt * (N - 2);

		float s0, s1, t0, t1;
		float tmp1, tmp2, x, y;

		float Nfloat = N;
		float ifloat, jfloat;
		int i, j;

		for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
			for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
				tmp1 = dtx * velocX[IX(i, j)];
				tmp2 = dty * velocY[IX(i, j)];
				x = ifloat - tmp1;
				y = jfloat - tmp2;

				if (x < 0.5f) x = 0.5f;
				if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
				i0 = floorf(x);
				i1 = i0 + 1.0f;
				if (y < 0.5f) y = 0.5f;
				if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
				j0 = floorf(y);
				j1 = j0 + 1.0f;

				s1 = x - i0;
				s0 = 1.0f - s1;
				t1 = y - j0;
				t0 = 1.0f - t1;

				int i0i = i0;
				int i1i = i1;
				int j0i = j0;
				int j1i = j1;

				//DOUBLE CHECK THIS
				d[IX(i, j)] =
					s0 * d0[IX(i0i, j0i)]
					+ t1 * d0[IX(i0i, j1i)]
					+ s1 * t0 * d0[IX(i1i, j0i)]
					+ t1 * d0[IX(i1i, j1i)];
			}
		}
		set_bnd(b, d);
	}


public:

	void addDensity(float x, float y, float amount)
	{
		std::lock_guard<std::mutex> lk(mutex);

		auto index = IX(x, y);
		fluid->density[index] += amount;
	}

	void addVelocity(float x, float y, float amountX, float amountY)
	{
		std::lock_guard<std::mutex> lk(mutex);

		auto index = IX(x, y);
		fluid->Vx[index] += amountX;
		fluid->Vy[index] += amountY;
	}

	void step()
	{
		std::lock_guard<std::mutex> lk(mutex);

		float visc = fluid->visc;
		float diff = fluid->diff;
		float dt = fluid->dt;
		float_val_array& Vx = fluid->Vx;
		float_val_array& Vy = fluid->Vy;
		float_val_array& Vx0 = fluid->Vx0;
		float_val_array& Vy0 = fluid->Vy0;
		float_val_array& s = fluid->s;
		float_val_array& density = fluid->density;

		diffuse(1, Vx0, Vx, visc, dt);
		diffuse(2, Vy0, Vy, visc, dt);

		project(Vx0, Vy0, Vx, Vy);

		advect(1, Vx, Vx0, Vx0, Vy0, dt);
		advect(2, Vy, Vy0, Vx0, Vy0, dt);

		project(Vx, Vy, Vx0, Vy0);

		diffuse(0, s, density, diff, dt);
		advect(0, density, s, Vx, Vy, dt);
	}

	void renderD(int scale)
	{
		std::lock_guard<std::mutex> lk(mutex);

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				float x = i * scale;
				float y = j * scale;
				float d = fluid->density[IX(i, j)];

				for (int xi = x; xi < x + scale; xi++)
				{
					for (int yi = y; yi < y + scale; yi++)
					{
						screen->setPixel(xi, yi, 255, d, d, 115);
					}
				}
			}
		}
	}

	void fadeD()
	{
		std::lock_guard<std::mutex> lk(mutex);

		std::for_each(fluid->density.begin(), fluid->density.end(), [](auto& el) {
			el = el - 0.1f; 
			if (el < 0) { el = 0; }
			else if (el > 255) { el = 255; }
			}
		);
	}

};
