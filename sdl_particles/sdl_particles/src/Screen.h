#pragma once

#include <memory>
#include <SDL.h>

namespace caveofprogramming
{

	class Screen
	{
	public:
		const static int SCREEN_WIDTH = 800;
		const static int SCREEN_HEIGHT = 600;

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
		bool init();
		void setPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue);
		void update();
		bool processEvents();
		void close();
		void clear();
		void boxBlur();
	};
} /* namespace caveofprogramming */
