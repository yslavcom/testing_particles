#pragma once

#include "BasicShapesDraw.h"
#include "BasicStructs.h"
#include "ErrorCode.h"
#include <vector>
#include <memory>


namespace SHAPES_2D
{
	using namespace TEST_SCREEN;

	/*
	https://www.modernescpp.com/index.php/c-core-guidelines-type-erasure-with-templates
	*/
	class BaseShape
	{
	public:
		template <typename T>
		BaseShape(const T& obj) : object(std::make_shared<Model<T>>(std::forward<decltype(obj)>(obj))) {}

		ErrorCode draw(pixel_vec_2d& pixel2d_buf)
		{
			return object->draw(pixel2d_buf);
		}

		ErrorCode update_window(Screen::ScreenWindow&& window)
		{
			return object->update_window(std::forward<decltype(window)>(window));
		}

		ErrorCode update_window(const Screen::ScreenWindow& window)
		{
			return object->update_window(window);
		}

		struct Concept {
			virtual ~Concept() {}
			virtual ErrorCode draw(pixel_vec_2d& pixel2d_buf) = 0;

			virtual ErrorCode update_window(Screen::ScreenWindow&& window) = 0;
			virtual ErrorCode update_window(const Screen::ScreenWindow& window) = 0;
		};

		template <typename T>
		struct Model : Concept
		{
			Model(const T& t) : object(t) {}

			ErrorCode draw(pixel_vec_2d& pixel2d_buf)  override {
				return object.draw(pixel2d_buf);
			}

			ErrorCode update_window(Screen::ScreenWindow&& window) override
			{
				return object.update_window(window);
			}

			ErrorCode update_window(const Screen::ScreenWindow& window) override
			{
				return object.update_window(window);
			}

		private:
			T object;
		};

		std::shared_ptr< Concept> object;
	};

	class Line
	{
		screen_ptr screen_;
		Screen::ScreenWindow window_;
		pixel_2d_coord_normal start_coord_;
		pixel_2d_coord_normal end_coord_;
		rgb_color_normalized colour_;
		float dash_len_norm_;
		float spacing_len_norm_;

	public:
		Line(
			screen_ptr screen,
			Screen::ScreenWindow window,
			pixel_2d_coord_normal start_coord, pixel_2d_coord_normal end_coord,
			rgb_color_normalized colour,
			float dash_len_norm, float spacing_len_norm)
			: screen_(screen)
			, window_(window)
			, start_coord_(start_coord)
			, end_coord_(end_coord)
			, colour_(colour)
			, dash_len_norm_(dash_len_norm)
			, spacing_len_norm_(spacing_len_norm)
		{}

		Line(const Line& other)
			: screen_(other.screen_)
			, window_(other.window_)
			, start_coord_(other.start_coord_)
			, end_coord_(other.end_coord_)
			, colour_(other.colour_)
			, dash_len_norm_(other.dash_len_norm_)
			, spacing_len_norm_(other.spacing_len_norm_)
		{}

	public:
		ErrorCode draw(pixel_vec_2d& pixel2d_buf)
		{
			auto rc = BasicShapesDraw::draw_dash_line(screen_,
				std::forward<decltype(window_)>(window_),
				pixel2d_buf,
				std::forward<decltype(start_coord_)>(start_coord_),
				std::forward<decltype(end_coord_)>(end_coord_),
				std::forward<decltype(colour_)>(colour_),
				dash_len_norm_, spacing_len_norm_);
			return rc;
		}

		auto update_window(Screen::ScreenWindow&& window)->ErrorCode
		{
			window_ = window;
			return ErrorCode::OK;
		}

		auto update_window(const Screen::ScreenWindow& window)->ErrorCode
		{
			window_ = window;
			return ErrorCode::OK;
		}
	};

	template <typename T>
	auto draw(T&& baseShape, pixel_vec_2d & pixel2d_buf) {
			return baseShape.draw(pixel2d_buf);
	}

	template <typename T>
	ErrorCode update_window(T& base, Screen::ScreenWindow && window)
	{
		return base.update_window(window);
	}

	template <typename T>
	ErrorCode update_window(T& base, const Screen::ScreenWindow& window)
	{
		return base.update_window(window);
	}
}
