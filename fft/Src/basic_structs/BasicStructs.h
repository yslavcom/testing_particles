#pragma once

#include <vector>
#include <SDL.h>

namespace BASIC_SHAPES_2D
{

	enum  class colour_name
	{
		RED,
		GREEN,
		BLUE,
		WHITE
	};

	class rgb_color_normalized
	{
	public:
		float r;
		float g;
		float b;

		rgb_color_normalized():r(0), g(0), b(0)
		{
		}
		rgb_color_normalized(const colour_name& color)
		{
			assign_color(color);
		}
		rgb_color_normalized(const colour_name&& color)
		{
			assign_color(color);
		}
		rgb_color_normalized operator=(const colour_name&& color)
		{
			assign_color(color);
			return *this;
		}


	private:
		inline void assign_color(const colour_name& color)
		{
			switch (color)
			{
			case colour_name::RED:
			{
				r = 1;
				g = 0;
				b = 0;
			}break;
			case colour_name::GREEN:
			{
				r = 0;
				g = 1;
				b = 0;
			}break;
			case colour_name::BLUE:
			{
				r = 0;
				g = 0;
				b = 1;
			}break;
			case colour_name::WHITE:
			default:
			{
				r = 1;
				g = 1;
				b = 1;
			}break;
			}
		}
	};

	class rgb_color
	{
	public:
		Uint8 r;
		Uint8 g;
		Uint8 b;

		rgb_color()
			:r(0), g(0), b(0)
		{}

		rgb_color(int r, int g, int b)
			:r(r), g(g), b(b)
		{}
	};

	template<typename T = float>
	class pixel_2d_coord_normalized
	{
	public:
		T hor;
		T ver;

		pixel_2d_coord_normalized()
			:hor(0), ver(0)
		{}

		pixel_2d_coord_normalized(T hor, T ver)
			:hor(hor), ver(ver)
		{}
	};

	using pixel_2d_coord_normal = pixel_2d_coord_normalized<>;

	class pixel_2d_coord
	{
	public:
		size_t hor;
		size_t ver;

		pixel_2d_coord() 
			:hor(0), ver(0)
		{}

		pixel_2d_coord(size_t hor, size_t ver)
			:hor(hor), ver(ver)
		{}

		pixel_2d_coord(int hor, int ver)
			:hor(hor), ver(ver)
		{}

		pixel_2d_coord(const pixel_2d_coord_normal && normalized, size_t width, size_t height)
			:hor(static_cast<size_t>(normalized.hor * width)), ver(static_cast<size_t>(normalized.ver * width))
		{}

		pixel_2d_coord(const pixel_2d_coord_normal&& normalized, int width, int height)
			:hor(static_cast<size_t>(normalized.hor* width)), ver(static_cast<size_t>(normalized.ver* width))
		{}
	};

	//using pixel_vec_2d = std::vector<std::vector<Uint32>>;

	struct pixel_vec_2d
	{
		enum { HOR_AXIS_IDX = 0 };
		enum { VER_AXIS_IDX = 1 };

	private:
		size_t m;
		size_t n;

	public:
		std::vector<std::vector<Uint32>> vec;

		pixel_vec_2d(size_t m, size_t n)
			: m(m), n(n) {
			vec[HOR_AXIS_IDX].reserve(m);
			vec[VER_AXIS_IDX].reserve(n);
		}

		pixel_vec_2d(int m, int n)
			: m(m), n(n) {
			vec[HOR_AXIS_IDX].reserve(m);
			vec[VER_AXIS_IDX].reserve(n);
		}

		inline size_t GET_WIDTH() const{
			return  m;
		}
		inline size_t GET_HEIGHT() const {
			return n;
		}

		bool operator()(const pixel_2d_coord&& coord, Uint32 val)
		{
			if (coord.hor < m
				&& coord.ver < n)
			{
				vec[coord.hor][coord.ver] = val;
				return true;
			}
			return false;
		}

		Uint32 operator()(const pixel_2d_coord&& coord)
		{
			if (coord.hor < m
				&& coord.ver < n)
			{
				return vec[coord.hor][coord.ver] ;
			}
			return 0;
		}
	};

}

