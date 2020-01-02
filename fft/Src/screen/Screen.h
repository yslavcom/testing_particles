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
		const static int MAX_ALPHA_VALUE = SDL_ALPHA_OPAQUE;
		const static int MAX_COLOR_VALUE = 255;

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

		static inline pixel_2d_coord convert_to_pixel_2d_coord(pixel_2d_coord_normal&& pixel_2d_coord_norm)
		{
			return { std::forward<decltype(pixel_2d_coord_norm)>(pixel_2d_coord_norm), Screen::SCREEN_WIDTH, Screen::SCREEN_HEIGHT };
		}

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
