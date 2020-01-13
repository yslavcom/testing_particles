#pragma once

#include<vector>
#include<map>
#include "Screen.h"
#include "ErrorCode.h"
#include "BasicShapes.h"

namespace BASIC_SHAPES_2D
{
	using namespace TEST_SCREEN;

	class Widget
	{
	private:
		screen_ptr screen_;
		Screen::ScreenWindow window_;

		std::vector<SHAPES_2D::BaseShape> shapes_vec_;

	public:
		Widget(const Screen::ScreenWindow& window, screen_ptr screen)
			:screen_(screen)
			, window_(window)
		{}

		Widget(const ScalingWindow& window, screen_ptr screen)
			:screen_(screen)
			, window_(window)
		{}

		Widget(Screen::ScreenWindow&& window, screen_ptr screen)
			:screen_(screen)
			, window_(window)
		{}

		Widget(ScalingWindow&& window, screen_ptr screen)
			:screen_(screen)
			, window_(window)
		{}

		virtual ~Widget() {}

		Widget(const Widget& other)
			:screen_(other.screen_)
			, window_(other.window_)
			, shapes_vec_(other.shapes_vec_)
		{}

		Widget(Widget&& other)
		{
			screen_ = std::move(other.screen_);
			window_ = std::move(other.window_);
			shapes_vec_ = std::move(other.shapes_vec_);
		}

		Widget& operator=(const Widget& other) = delete;
		Widget& operator=(Widget&& other) = delete;

		template<typename T>
		size_t add_shape(T&& shape)
		{
			size_t index = shapes_vec_.size();
			shapes_vec_.emplace_back(shape);
			return index;
		}

		void delete_shape_by_index(size_t index)
		{
			shapes_vec_.erase(shapes_vec_.begin() + index);
		}

		template<typename W>
		ErrorCode update_window(W&& window)
		{
			window_ = window;
			return ErrorCode::OK;
		}

		ErrorCode move_window(pixel_2d_coord&& coord)
		{
			window_.corner_coord = coord;
			return ErrorCode::OK;
		}

		ErrorCode move_window(const pixel_2d_coord& coord)
		{
			window_.corner_coord = coord;
			return ErrorCode::OK;
		}

		ErrorCode draw(pixel_vec_2d& pixel2d_buf)
		{
			for (auto v : shapes_vec_)
			{
				v.draw(screen_, window_, pixel2d_buf);
			}

			return ErrorCode::OK;
		}
	};

	struct WidgetWindows
	{
		std::vector<Screen::ScreenWindow> windows;
		std::map<Screen::ScreenWindow, const Widget&> map_widget;

		template<typename COORD>
		auto get_widget(COORD&& click_coord)
		{
			auto result = std::accumulate(windows.begin()
				, windows.end()
				, 0
				, [=](std::vector<COORD>vec, auto& w)->int
				{
					if (click_coord->hor >= w.get_vertex_coord(Screen::ScreenWindow::Vertex::A).hor
						&& click_coord->hor <= w.get_vertex_coord(Screen::ScreenWindow::Vertex::B).hor)
					{
						if (click_coord->ver >= w.get_vertex_coord(Screen::ScreenWindow::Vertex::C).ver
							&& click_coord->ver <= w.get_vertex_coord(Screen::ScreenWindow::Vertex::A).ver)
						{
							vec.emplace_back(w);
							DebugLog::instance()->print("window clicked");
						}
					}
					return vec;
				});
		}
	};

	template <typename T, typename COORD>
	auto move_widget(T&& obj, COORD&& coord)
	{
		return obj.move_window(coord);
	}
}
