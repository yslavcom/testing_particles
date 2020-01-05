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

		struct ScreenWindow
		{
			enum class Vertex
			{
				A, B, C, D
			};

			pixel_2d_coord corner_coord;
			size_t w;
			size_t h;

			ScreenWindow(const pixel_2d_coord& corner_coord, size_t w, size_t h)
				:corner_coord(corner_coord)
				, w(w)
				, h(h)
			{}

			inline pixel_2d_coord get_vertex_coord(Vertex vertex)
			{
				/*
				D**C
				*  *
				*  *
				A**B
				*/

				switch (vertex)
				{
				case Vertex::A:
					return corner_coord;

				case Vertex::B:
					return { corner_coord.hor + w, corner_coord.ver };

				case Vertex::C:
					return { corner_coord.hor + w, corner_coord.ver - h };

				case Vertex::D:
					return { corner_coord.hor , corner_coord.ver - h};
				}
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

		template<typename W>
		static inline ScreenWindow to_screen_window(W&& window)
		{
			ScalingWindow w(window);
			auto w_coord = pixel_2d_coord{ std::forward<decltype(w.corner_coord)>(w.corner_coord), Screen::SCREEN_WIDTH, Screen::SCREEN_HEIGHT };
			size_t w_w = w.w * Screen::SCREEN_WIDTH;
			size_t w_h = w.h * Screen::SCREEN_HEIGHT;

			return { w_coord , w_w, w_h };
		}

		template<typename T, typename W>
		static inline pixel_2d_coord convert_to_pixel_2d_coord(T&& pixel_2d_coord_norm, W&& window)
		{
			/*
			Get the window area on the screen
			*/
			ScreenWindow screenWindow(to_screen_window(window));
			
			auto coord = pixel_2d_coord{ std::forward<pixel_2d_coord_normal>(pixel_2d_coord_norm), screenWindow.w, screenWindow.h };
			coord.hor += screenWindow.corner_coord.hor;
			coord.ver = screenWindow.corner_coord.ver - (screenWindow.h - coord.ver);

			return coord;
		}

		template<typename W>
		static inline size_t convert_normalized_len_to_screen_len(W&& window, float len, float tangent)
		{
			double tan = static_cast<double>(tangent);
			double cosine = 1.0 / std::sqrt(1.0 + tan * tan); // we do not care about sign in this case
			double sine = std::sqrt(1.0 - cosine * cosine); // we do not care about sign in this case

			ScreenWindow screenWindow(to_screen_window(window));
			auto x = screenWindow.w * cosine;
			auto y = screenWindow.h * sine;
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
