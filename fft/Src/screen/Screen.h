#pragma once

#include <memory>
#include <variant>
#include <optional>

#include <SDL.h>

#include "ErrorCode.h"
#include "../debug_log/DebugLog.h"

namespace TEST_SCREEN
{
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

		struct coord
		{
			int x;
			int y;

			coord() :x(0), y(0)
			{}


			coord(float x, float y)
				:x(static_cast<int>(x* SCREEN_WIDTH)),
				y(static_cast<int>(y* SCREEN_HEIGHT))
			{}

			coord(int x, int y)
				:x(x),
				y(y)
			{}
		};

		struct color
		{
			int r;
			int g;
			int b;

			color(float r, float g, float b)
				:r(static_cast<int>(r* MAX_COLOR_VALUE)),
				g(static_cast<int>(g* MAX_COLOR_VALUE)),
				b(static_cast<int>(b* MAX_COLOR_VALUE))
			{}

			color(int r, int g, int b)
				:r(r),
				g(g),
				b(b)
			{}
		};

		using EventContainer = std::variant<coord, std::monostate>;

	private:
		class ScreenResources;
		std::unique_ptr<ScreenResources> screenResources;

		inline bool validate_screen_bounds(const Screen::coord&& coord)
		{
			if (coord.x < 0
				|| coord.x >= Screen::SCREEN_WIDTH
				|| coord.y < 0
				|| coord.y >= Screen::SCREEN_HEIGHT) {
				//std::cerr << "Warning: this check is not time efficient" << std::endl;
				return false;
			}

			return true;
		}

	public:
		Screen();
		virtual ~Screen();
		ErrorCode init(int w, int h);
		ErrorCode setPixel(const Screen::coord&& coord, Uint8 alpha, const Screen::color&& color);
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
