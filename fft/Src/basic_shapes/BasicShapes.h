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

		ErrorCode draw(screen_ptr screen, Screen::ScreenWindow&& window, pixel_vec_2d& pixel2d_buf)
		{
			return object->draw(screen, window, pixel2d_buf);
		}

		ErrorCode draw(screen_ptr screen, const Screen::ScreenWindow& window, pixel_vec_2d& pixel2d_buf)
		{
			return object->draw(screen, window, pixel2d_buf);
		}

		struct Concept {
			virtual ~Concept() {}
			virtual ErrorCode draw(screen_ptr screen, Screen::ScreenWindow&& window, pixel_vec_2d& pixel2d_buf) = 0;
			virtual ErrorCode draw(screen_ptr screen, const Screen::ScreenWindow& window, pixel_vec_2d& pixel2d_buf) = 0;
		};

		template <typename T>
		struct Model : Concept
		{
			Model(const T& t) : object(t) {}

			ErrorCode draw(screen_ptr screen, Screen::ScreenWindow&& window, pixel_vec_2d& pixel2d_buf)  override {
				return object.draw(screen, window, pixel2d_buf);
			}

			ErrorCode draw(screen_ptr screen, const Screen::ScreenWindow& window, pixel_vec_2d& pixel2d_buf)  override {
				return object.draw(screen, window, pixel2d_buf);
			}


		private:
			T object;
		};

		std::shared_ptr< Concept> object;
	};

	template <typename T>
	auto draw(T&& baseShape, screen_ptr screen, Screen::ScreenWindow&& window, pixel_vec_2d& pixel2d_buf) {
			return baseShape.draw(screen, window, pixel2d_buf);
	}

	template <typename T>
	auto draw(T&& baseShape, screen_ptr screen, const Screen::ScreenWindow& window, pixel_vec_2d& pixel2d_buf) {
		return baseShape.draw(screen, window, pixel2d_buf);
	}
}
