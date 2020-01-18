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
		rgb_color_normalized(const rgb_color_normalized& colour)
			:r(colour.r)
			, g(colour.g)
			, b(colour.b)
		{
		}
		rgb_color_normalized(rgb_color_normalized&& colour)noexcept
		{
			r = std::move(colour.r);
			g = std::move(colour.g);
			b = std::move(colour.b);
		}
		rgb_color_normalized& operator=(const rgb_color_normalized& other)
		{
			r = other.r;
			g = other.g;
			b = other.b;
			return *this;
		}
		rgb_color_normalized& operator=(rgb_color_normalized&& other)
		{
			r = std::move(other.r);
			g = std::move(other.g);
			b = std::move(other.b);
			return *this;
		}

		rgb_color_normalized(float r, float g, float b) :r(r), g(g), b(b)
		{
		}
		rgb_color_normalized(double r, double g, double b) :r(static_cast<float>(r)), g(static_cast<float>(g)), b(static_cast<float>(b))
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
		rgb_color_normalized& operator=(const colour_name&& color)
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

		pixel_2d_coord_normalized(double hor, double ver)
			:hor(static_cast<T>(hor)), ver(static_cast<T>(ver))
		{}

		pixel_2d_coord_normalized(const pixel_2d_coord_normalized& other)
		{
			hor = other.hor;
			ver = other.ver;
		}

		pixel_2d_coord_normalized(pixel_2d_coord_normalized&& other)
		{
			hor = std::move(other.hor);
			ver = std::move(other.ver);
		}

		pixel_2d_coord_normalized& operator=(const pixel_2d_coord_normalized& other)
		{
			hor = other.hor;
			ver = other.ver;
			return *this;
		}

		pixel_2d_coord_normalized& operator=(pixel_2d_coord_normalized&& other)noexcept
		{
			hor = std::move(other.hor);
			ver = std::move(other.ver);
			return *this;
		}
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

		pixel_2d_coord( pixel_2d_coord_normal && normalized, size_t width, size_t height)
			:hor(static_cast<size_t>((normalized.hor) * width)), ver(static_cast<size_t>((1-normalized.ver) * height))
		{}

		pixel_2d_coord( pixel_2d_coord_normal&& normalized, int width, int height)
			:hor(static_cast<size_t>((normalized.hor)* width)), ver(static_cast<size_t>((1-normalized.ver)* height))
		{}

		pixel_2d_coord(const pixel_2d_coord_normal& normalized, int width, int height)
			:hor(static_cast<size_t>((normalized.hor)* width)), ver(static_cast<size_t>((1 - normalized.ver)* height))
		{}

		pixel_2d_coord(const pixel_2d_coord& other)
		{
			hor = other.hor;
			ver = other.ver;
		}

		pixel_2d_coord(pixel_2d_coord&& other)noexcept
		{
			hor = std::move(other.hor);
			ver = std::move(other.ver);
		}

		pixel_2d_coord& operator=(const pixel_2d_coord& other)
		{
			hor = other.hor;
			ver = other.ver;
			return *this;
		}

		pixel_2d_coord& operator=(pixel_2d_coord&& other)noexcept
		{
			hor = std::move(other.hor);
			ver = std::move(other.ver);
			return *this;
		}
	};

	//using pixel_vec_2d = std::vector<std::vector<Uint32>>;

	struct pixel_vec_2d
	{
	private:
		size_t m;
		size_t n;

		std::unique_ptr<Uint32[]>ptr;
		inline size_t size_in_bytes()const 
		{
			return (m * n * sizeof(Uint32));
		}

	public:
		pixel_vec_2d()
			:m(0), n(0)
		{}
		pixel_vec_2d(pixel_vec_2d& other) = delete;
		pixel_vec_2d& operator=(pixel_vec_2d& other) = delete;

		pixel_vec_2d& operator=(pixel_vec_2d&& other)
		{
			m = std::move(other.m);
			n = std::move(other.n);
			ptr = std::move(other.ptr);
			other.ptr = nullptr;
			return *this;
		}

		pixel_vec_2d(pixel_vec_2d&& other)
			: m(std::move(other.m))
			, n(std::move(other.n))
			, ptr(std::move(other.ptr))
		{
			other.ptr = nullptr;
		}

		pixel_vec_2d(size_t m, size_t n)
			: m(m), n(n) {
			ptr = std::make_unique<Uint32[]>(m*n);
		}

		pixel_vec_2d(int m, int n)
			: m(m), n(n) {
			ptr = std::make_unique<Uint32[]>(m * n);
		}

		void init(int in_m, int in_n)
		{
			m = in_m;
			n = in_n;

			ptr = std::make_unique<Uint32[]>(m * n);
		}

		void set_fill(uint8_t val) // for debug purposes
		{
			auto ptr = this->ptr.get();

			for (auto x = 0; x < m; x++)
			{
				for (auto y = 0; y < n; y++)
				{
					ptr[x * 4 + y] = val;
				}
			}
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

		Uint32 operator()( pixel_2d_coord&& coord)
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
			
			auto copy_size = std::min(buffer_size_bytes, size_in_bytes());
			auto src_ptr = this->ptr.get();
			SDL_memcpy(ptr, src_ptr, copy_size);

			return true;
		}

		void clear()
		{
			SDL_memset(ptr.get(), 0, size_in_bytes());
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

	static Uint32 to_colour_word(rgb_color_normalized&& color, float alpha)
	{
		Uint32 colour = 0;

		colour += static_cast<Uint8>(alpha * MAX_ALPHA_VALUE);
		colour <<= 8;
		colour += static_cast<Uint8>(color.r * MAX_COLOR_VALUE);
		colour <<= 8;
		colour += static_cast<Uint8>(color.g * MAX_COLOR_VALUE);
		colour <<= 8;
		colour += static_cast<Uint8>(color.b * MAX_COLOR_VALUE);

		return colour;
	}

	struct ScalingWindow
	{
		pixel_2d_coord_normal corner_coord;
		float w;
		float h;

		ScalingWindow()
			: w(1)
			, h(1)
			, corner_coord(0.0, 0.0)
		{}

		ScalingWindow(pixel_2d_coord_normal corner_coord, float w, float h)
			: w(w)
			, h(h)
			, corner_coord(corner_coord)
		{}

		ScalingWindow(const ScalingWindow& other)
		{
			corner_coord = other.corner_coord;
			w = other.w;
			h = other.h;
		}

		ScalingWindow(ScalingWindow&& other)noexcept
		{
			corner_coord = std::move(other.corner_coord);
			w = std::move(other.w);
			h = std::move(other.h);
		}

		ScalingWindow& operator=(const ScalingWindow& other)
		{
			corner_coord = other.corner_coord;
			w = other.w;
			h = other.h;
			return *this;
		}

		ScalingWindow& operator=(ScalingWindow&& other)noexcept
		{
			corner_coord = std::move(other.corner_coord);
			w = std::move(other.w);
			h = std::move(other.h);
			return *this;
		}
	};

}

