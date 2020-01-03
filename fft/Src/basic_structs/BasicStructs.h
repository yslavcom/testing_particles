#pragma once

#include <algorithm>
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
		rgb_color_normalized(float r, float g, float b) :r(r), g(g), b(b)
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
			:hor(static_cast<size_t>(normalized.hor * width)), ver(static_cast<size_t>(normalized.ver * height))
		{}

		pixel_2d_coord(const pixel_2d_coord_normal&& normalized, int width, int height)
			:hor(static_cast<size_t>(normalized.hor* width)), ver(static_cast<size_t>(normalized.ver* height))
		{}
	};

	//using pixel_vec_2d = std::vector<std::vector<Uint32>>;

	struct pixel_vec_2d
	{
	private:
		size_t m;
		size_t n;

		std::unique_ptr<Uint32[]>ptr;
		size_t size_in_bytes;

	public:
		pixel_vec_2d(size_t m, size_t n)
			: m(m), n(n) {
			ptr = std::make_unique<Uint32[]>(m*n);
			size_in_bytes = m * n * sizeof(Uint32);
		}

		pixel_vec_2d(int m, int n)
			: m(m), n(n) {
			ptr = std::make_unique<Uint32[]>(m * n);
			size_in_bytes = m * n * sizeof(Uint32);
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
				ptr[m * coord.ver + coord.hor] = val;
				return true;
			}
			return false;
		}

		Uint32 operator()(const pixel_2d_coord&& coord)
		{
			if (coord.hor < m
				&& coord.ver < n)
			{
				return ptr[m * coord.ver + coord.hor];
			}
			return 0;
		}

		bool copy_to_plain_buf(Uint32* ptr, size_t buffer_size_bytes) const
		{
			if (nullptr == ptr)return false;
			
			auto copy_size = std::min(buffer_size_bytes, size_in_bytes);
			SDL_memcpy(ptr, this->ptr.get(), copy_size);

			return true;
		}
	};


	const static int MAX_ALPHA_VALUE = SDL_ALPHA_OPAQUE;
	const static int MAX_COLOR_VALUE = 255;
	static Uint32 to_colour_word(rgb_color&& color, Uint8 alpha)
	{
		Uint32 colour = 0;

		colour += alpha;
		colour <<= 8;
		colour += color.r;
		colour <<= 8;
		colour += color.g;
		colour <<= 8;
		colour += color.b;

		return colour;
	}

	static Uint32 to_colour_word(rgb_color_normalized&& color, Uint8 alpha)
	{
		Uint32 colour = 0;

		colour += alpha;
		colour <<= 8;
		colour += (color.r * MAX_COLOR_VALUE);
		colour <<= 8;
		colour += (color.g * MAX_COLOR_VALUE);
		colour <<= 8;
		colour += (color.b * MAX_COLOR_VALUE);

		return colour;
	}
}

