#pragma once

#include<vector>
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

		void delete_shape(size_t index)
		{
			shapes_vec_.erase(shapes_vec_.begin() + index);
		}

		template<typename W>
		ErrorCode update_window(W&& window)
		{
			window_ = window;
			for (auto v : shapes_vec_)
			{
				v.update_window(window);
			}

			return ErrorCode::OK;
		}

		ErrorCode draw(pixel_vec_2d& pixel2d_buf)
		{
			for (auto v : shapes_vec_)
			{
				v.draw(pixel2d_buf);
			}

			return ErrorCode::OK;
		}
	};
}
