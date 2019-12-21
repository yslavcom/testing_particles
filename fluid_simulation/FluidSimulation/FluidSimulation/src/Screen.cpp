#include <iostream>
#include "Screen.h"

#if 1
	template<typename Creator, typename Destructor, typename... Arguments>
	auto make_resource(Creator c, Destructor d, Arguments&&... args)
	{
		auto r = c(std::forward<Arguments>(args)...);
		if (!r) { throw std::system_error(errno, std::generic_category()); }
		return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
	}


	auto make_window(
		const char* title, int x, int y, int w, int h, Uint32 flags) {
		return make_resource(SDL_CreateWindow, SDL_DestroyWindow,
			title, x, y, w, h, flags);
	}

	auto make_renderer(
		SDL_Window* window, int index, Uint32 flags) {
		return make_resource(SDL_CreateRenderer, SDL_DestroyRenderer,
			window, index, flags);
	}

	auto make_texture(
		SDL_Renderer* renderer,
		Uint32 format,
		int access, int w,
		int h) {
		return make_resource(SDL_CreateTexture, SDL_DestroyTexture,
			renderer,
			format,
			access, w,
			h);
	}
#endif
	int Screen::SCREEN_WIDTH = 800;
	int Screen::SCREEN_HEIGHT = 600;

	Screen::Screen() :
			m_window(nullptr, nullptr),
			m_renderer(nullptr, nullptr),
			m_texture(nullptr, nullptr),
			m_buffer1(nullptr),
			m_buffer2(nullptr)
		{

			if (SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				std::cout << "SDL failed" << std::endl;
			}
			else
			{
				std::cout << "SDL succeeded" << std::endl;
			}
		}

	Screen::~Screen()
	{
		//call destruction of the other SDL-related resources to ensure proper order
		//xxx
		close();
	}


		bool Screen::init(int w, int h)
		{
			SCREEN_WIDTH = w;
			SCREEN_HEIGHT = h;

			m_window = make_window("SDL window",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH, SCREEN_HEIGHT,
				SDL_WINDOW_SHOWN
			);

			m_renderer = make_renderer(m_window.get(), -1, SDL_RENDERER_PRESENTVSYNC);

			m_texture = make_texture(m_renderer.get(),
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
				SCREEN_WIDTH, SCREEN_HEIGHT
			);

			m_buffer1 = std::make_unique<Uint32[]>(SCREEN_WIDTH * SCREEN_HEIGHT);
			m_buffer2 = std::make_unique<Uint32[]>(SCREEN_WIDTH * SCREEN_HEIGHT);

			auto raw_buffer = m_buffer1.get();
			memset(raw_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
			raw_buffer = m_buffer2.get();
			memset(raw_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

			return true;
		}

		void Screen::setPixel(int x, int y, Uint8 alpha, Uint8 red, Uint8 green, Uint8 blue)
		{
			if (x < 0
				|| x >= Screen::SCREEN_WIDTH
				|| y < 0
				|| y >= Screen::SCREEN_HEIGHT) {
				//std::cerr << "Warning: this check is not time efficient" << std::endl;
				return;
			}

			Uint32 colour = 0;

			colour += red;
			colour <<= 8;
			colour += green;
			colour <<= 8;
			colour += blue;
			colour <<= 8;
			colour += alpha; //alpha

			m_buffer1.get()[(y * SCREEN_WIDTH) + x] = colour;
		}

		void Screen::update()
		{
			auto raw_buffer = m_buffer1.get();
			SDL_UpdateTexture(m_texture.get(), NULL, raw_buffer, SCREEN_WIDTH * sizeof(Uint32));

			SDL_RenderClear(m_renderer.get());
			SDL_RenderCopy(m_renderer.get(), m_texture.get(), NULL, NULL);
			SDL_RenderPresent(m_renderer.get());

		}


		std::optional<std::pair<Screen::EventType, Screen::EventContainer>> Screen::processEvents()
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
				{
					return std::make_pair<EventType, EventContainer>(Screen::EventType::Quit, std::monostate());
				}break;

				case SDL_MOUSEBUTTONDOWN:
				{
					if (SDL_BUTTON(SDL_BUTTON_LEFT) == event.button.button)
					{
						coord coord;
						coord.x = event.button.x;
						coord.y = event.button.y;
						return std::make_pair<EventType, EventContainer>(Screen::EventType::LeftMouseDown, coord);
					}
				}break;

				case SDL_MOUSEMOTION:
				{
					if (SDL_BUTTON_LMASK & event.motion.state)
					{
						coord coord;
						coord.x = event.motion.x;
						coord.y = event.motion.y;
						return std::make_pair<EventType, EventContainer>(Screen::EventType::MouseDragging, coord);
					}
				}
				}break;

			}
			return {};
		}

		bool Screen::mouseDragging(int& x, int& y)
		{
			return false;
		}

		void Screen::close()
		{
			SDL_Quit();
		}

		void Screen::clear()
		{
			memset(m_buffer1.get(), 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
			memset(m_buffer2.get(), 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
		}

		void Screen::boxBlur()
		{
			std::swap(m_buffer1, m_buffer2);

			for (int y = 0; y < Screen::SCREEN_HEIGHT; y++)
			{
				for (int x = 0; x < Screen::SCREEN_WIDTH; x++)
				{

					int redTotal = 0;
					int greenTotal = 0;
					int blueTotal = 0;

					for (int row = -1; row <= 1; row++)
					{
						for (int col = -1; col <= 1; col++)
						{
							int currentX = x + col;
							int currentY = y + row;

							if (currentX >= 0 && currentX < Screen::SCREEN_WIDTH
								&& currentY >= 0 && currentY < Screen::SCREEN_HEIGHT)
							{
								Uint32 colour = m_buffer2[currentY * Screen::SCREEN_WIDTH + currentX];

								Uint8 red = colour >> 24;
								Uint8 green = colour >> 16;
								Uint8 blue = colour >> 8;

								redTotal += red;
								greenTotal += green;
								blueTotal += blue;
							}
						}
					}

					Uint8 red = redTotal / 9;
					Uint8 green = greenTotal / 9;
					Uint8 blue = blueTotal / 9;

					setPixel(x, y, 255, red, green, blue);
				}
			}
		}
