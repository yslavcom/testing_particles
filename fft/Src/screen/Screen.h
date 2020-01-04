#pragma once

#include <memory>
#include <variant>
#include <optional>

#include <SDL.h>

#include "ErrorCode.h"
#include "DebugLog.h"
#include "BasicStructs.h"

namespace TEST_SCREEN
{
	using namespace BASIC_SHAPES_2D;

	class Screen
	{
	public:
		struct Window
		{
			pixel_2d_coord_normal corner_coord;
			float w;
			float h;

			Window()
				: w(1)
				, h(1)
				, corner_coord(0.0, 0.0)
			{}

			Window(pixel_2d_coord_normal corner_coord, float w, float h)
				: w(w)
				, h(h)
				, corner_coord(corner_coord)
			{}

			Window(const Window& other)
			{
				corner_coord = other.corner_coord;
				w = other.w;
				h = other.h;
			}

			Window( Window&& other)
			{
				corner_coord = std::move(other.corner_coord);
				w = std::move(other.w);
				h = std::move(other.h);
			}

			Window operator=(const Window& other)
			{
				*this = other;
				return *this;
			}

			Window operator=( Window&& other)
			{
				*this = std::move(other);
				return *this;
			}
		};

		static int SCREEN_WIDTH;
		static int SCREEN_HEIGHT;

		enum  class EventType
		{
			LeftMouseDown, // left mouse button pressed
			MouseDragging, // left mouse button down & mouse is moving
			Quit, // quit app event
		};

		using EventContainer = std::variant<pixel_2d_coord, std::monostate>;

	private:
		class ScreenResources;
		std::unique_ptr<ScreenResources> screenResources;

	public:
		Screen();
		virtual ~Screen();
		ErrorCode init(int w, int h);

		template<typename T, typename W>
		static inline pixel_2d_coord convert_to_pixel_2d_coord(T&& pixel_2d_coord_norm, W&& window)
		{
			/*
			Get the window area on the screen
			*/
			Screen::Window w(window);
			auto w_coord = pixel_2d_coord{ std::forward<decltype(w.corner_coord)>(w.corner_coord), Screen::SCREEN_WIDTH, Screen::SCREEN_HEIGHT };
			size_t w_w = w.w * Screen::SCREEN_WIDTH;
			size_t w_h = w.h * Screen::SCREEN_HEIGHT;
			
			auto coord = pixel_2d_coord{ std::forward<pixel_2d_coord_normal>(pixel_2d_coord_norm), w_w, w_h };
			coord.hor += w_coord.hor;
			coord.ver = w_coord.ver - (w_h - coord.ver);

			return coord;
		}

		template<typename W>
		static inline size_t convert_normalized_len_to_screen_len(W&& window, float len, float tangent)
		{
			double tan = static_cast<double>(tangent);
			double cosine = 1.0 / std::sqrt(1.0 + tan * tan); // we do not care about sign in this case
			double sine = std::sqrt(1.0 - cosine * cosine); // we do not care about sign in this case

			Screen::Window w(window);
			auto x = w.w * Screen::SCREEN_WIDTH * cosine;
			auto y = w.h * Screen::SCREEN_HEIGHT * sine;
			auto d = std::sqrt((x * x) + (y * y));

			return static_cast<size_t>(std::ceil(len * d));
		}

		void copy_to_screen_buf(const pixel_vec_2d& pixel_vec_2d);
		void clear_render();
		void update_from_pixel_buffer();
		void present_render();
		std::optional<std::pair<EventType, EventContainer>> processEvents();
		void close();
		void clear();
		void boxBlur();

	};

	using screen_ptr = std::shared_ptr<Screen>;
}
