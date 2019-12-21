#pragma once

#include <memory>
#include <variant>
#include <optional>

#include <SDL.h>



class Screen
{
public:
	static int SCREEN_WIDTH;
	static int SCREEN_HEIGHT;

	enum  class EventType
	{
		LeftMouseDown, // left mouse button pressed
		MouseDragging, // left mouse button down & mouse is moving
		Quit , // quit app event
	};

	struct coord
	{
		int x;
		int y;
	};

	using EventContainer = std::variant<coord, std::monostate>;

private:

	using window_ptr_t = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
	using renderer_ptr_t = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
	using texture_ptr_t = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

	window_ptr_t m_window;
	renderer_ptr_t m_renderer;
	texture_ptr_t m_texture;
	std::unique_ptr<Uint32[]> m_buffer1;
	std::unique_ptr<Uint32[]> m_buffer2;

public:
	Screen();
	virtual ~Screen();
	bool init(int w, int h);
	void setPixel(int x, int y, Uint8 alpha, Uint8 red, Uint8 green, Uint8 blue);
	void update();
	std::optional<std::pair<EventType, EventContainer>> processEvents();
	bool mouseDragging(int& x, int& y);
	void close();
	void clear();
	void boxBlur();
};

