#pragma once

#include <memory>
#include <variant>
#include <optional>

#include <SDL.h>

#include "ErrorCode.h"
#include "DebugLog.h"
#include "BasicStructs.h"
#include "Pipe.h"

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
			int32_t w;
			int32_t h;

			ScreenWindow()
				:corner_coord(), w(0), h(0)
			{}

			ScreenWindow(const pixel_2d_coord& corner_coord, int32_t w, int32_t h)
				:corner_coord(corner_coord)
				, w(w)
				, h(h)
			{}

			ScreenWindow(const ScalingWindow& window)
				:corner_coord(pixel_2d_coord{ window.corner_coord, Screen::SCREEN_WIDTH, Screen::SCREEN_HEIGHT })
				, w(static_cast<int32_t>(window.w* Screen::SCREEN_WIDTH))
				, h(static_cast<int32_t>(window.h* Screen::SCREEN_HEIGHT))
			{}

			ScreenWindow(const ScreenWindow& other)
				:corner_coord(other.corner_coord)
				, w(other.w)
				, h(other.h)
			{}

			ScreenWindow(ScreenWindow&& other)noexcept
			{
				corner_coord = std::move(other.corner_coord);
				w = std::move(other.w);
				h = std::move(other.h);
			}

			ScreenWindow& operator=(const ScreenWindow& other)
			{
				corner_coord = other.corner_coord;
				w = other.w;
				h = other.h;
				return *this;
			}

			ScreenWindow& operator=(ScreenWindow&& other)noexcept
			{
				corner_coord = std::move(other.corner_coord);
				w = std::move(other.w);
				h = std::move(other.h);
				return *this;
			}

			inline pixel_2d_coord get_vertex_coord(Vertex vertex)const 
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

			bool inside(const pixel_2d_coord& coord)const
			{
				if (coord.hor >= get_vertex_coord(Screen::ScreenWindow::Vertex::A).hor
					&& coord.hor <= get_vertex_coord(Screen::ScreenWindow::Vertex::B).hor)
				{
					if (coord.ver >= get_vertex_coord(Screen::ScreenWindow::Vertex::C).ver
						&& coord.ver <= get_vertex_coord(Screen::ScreenWindow::Vertex::A).ver)
					{
						return true;
					}
				}

				return false;
			}
		};

		static int SCREEN_WIDTH;
		static int SCREEN_HEIGHT;

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
			ScreenWindow w(window);
			return w;
		}

		template<typename W>
		static inline ScreenWindow to_screen_window(W& window)
		{
			ScreenWindow w(window);
			return w;
		}

		template<typename T, typename W>
		static inline pixel_2d_coord convert_normalized_to_pixel_2d_coord(T&& pixel_2d_coord_norm, W&& window)
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

		template<typename T, typename W>
		static inline pixel_2d_coord convert_with_pixel_2d_coord(T&& pixel_coord, W&& window)
		{
			/*
			Get the window area on the screen
			*/
			ScreenWindow screenWindow{ window };

			auto coord = pixel_2d_coord{ pixel_coord };
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

		template<typename W>
		static inline size_t convert_len_to_screen_len(W&& window, float len, float tangent)
		{
			double tan = static_cast<double>(tangent);
			double cosine = 1.0 / std::sqrt(1.0 + tan * tan); // we do not care about sign in this case
			double sine = std::sqrt(1.0 - cosine * cosine); // we do not care about sign in this case

			ScreenWindow screenWindow(window);
			auto x = screenWindow.w * cosine;
			auto y = screenWindow.h * sine;
			auto d = std::sqrt((x * x) + (y * y));

			return static_cast<size_t>(std::ceil(len * d));
		}

		void copy_to_screen_buf(const pixel_vec_2d& pixel_vec_2d);
		void clear_render();
		void update_from_pixel_buffer();
		void present_render();
		void close();
		void clear();
		void boxBlur();

	};

	using screen_ptr = std::shared_ptr<Screen>;
}
