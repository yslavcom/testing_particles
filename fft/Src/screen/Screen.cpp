#include <iostream>
#include "Screen.h"

namespace TEST_SCREEN
{
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

	class Screen::ScreenResources
	{
	private:
		using window_ptr_t = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
		using renderer_ptr_t = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
		using texture_ptr_t = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

		window_ptr_t m_window;
		texture_ptr_t m_texture;

		int SCREEN_WIDTH;
		int SCREEN_HEIGHT;

	public:
		renderer_ptr_t m_renderer;
		std::unique_ptr<Uint32[]> m_buffer1;
		std::unique_ptr<Uint32[]> m_buffer2;

	public:
		ScreenResources() :
			m_window(nullptr, nullptr),
			m_renderer(nullptr, nullptr),
			m_texture(nullptr, nullptr),
			m_buffer1(nullptr),
			m_buffer2(nullptr)
		{}

		ErrorCode init(int w, int h)
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
				SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
				SCREEN_WIDTH, SCREEN_HEIGHT
			);

			m_buffer1 = std::make_unique<Uint32[]>(SCREEN_WIDTH * SCREEN_HEIGHT);
			m_buffer2 = std::make_unique<Uint32[]>(SCREEN_WIDTH * SCREEN_HEIGHT);

			auto raw_buffer = m_buffer1.get();
			memset(raw_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
			raw_buffer = m_buffer2.get();
			memset(raw_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

			return ErrorCode::OK;
		}

		inline void clear_render()
		{
			SDL_RenderClear(m_renderer.get());
		}

		inline void update_from_pixel_buffer()
		{
#if 1
			auto tex = m_texture.get();
			void* pixels = nullptr;
			int pitch = 0;
			auto res = SDL_LockTexture(tex, NULL, &pixels, &pitch);
			if (0 != res)
			{
				DebugLog::instance()->print(SDL_GetError());
			}
			else
			{
				if (nullptr != pixels)
				{
					SDL_memcpy(pixels, m_buffer1.get(), pitch * SCREEN_HEIGHT);
				}
				SDL_UnlockTexture(tex);
				SDL_RenderCopy(m_renderer.get(), tex, NULL, NULL);
			}
#else
			auto tex = m_texture.get();
			SDL_UpdateTexture(tex, NULL, m_buffer1.get(), SCREEN_WIDTH * sizeof(Uint32));
			SDL_RenderCopy(m_renderer.get(), tex, NULL, NULL);
#endif
		}

		inline void present_render()
		{
			SDL_RenderPresent(m_renderer.get());
		}
	};


	int Screen::SCREEN_WIDTH = 800;
	int Screen::SCREEN_HEIGHT = 600;

	Screen::Screen()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			DebugLog::instance()->print("SDL init failed");
		}
		else
		{
			DebugLog::instance()->print("SDL init succeeded");
		}

		screenResources = std::make_unique<ScreenResources>();
	}

	Screen::~Screen()
	{
		//call destruction of the other SDL-related resources to ensure proper order
		screenResources.reset();
		close();
	}


	ErrorCode Screen::init(int w, int h)
	{
		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;
		return screenResources->init(SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	ErrorCode Screen::setPixel(const Screen::coord&& coord, Uint8 alpha, const Screen::color&& color)
	{
		if (!validate_screen_bounds(std::forward<const Screen::coord>(coord)))
		{
			return ErrorCode::SET_PIXEL_OUT_OF_BOUNDS;
		}

		Uint32 colour = 0;

		colour += alpha;
		colour <<= 8;
		colour += color.r;
		colour <<= 8;
		colour += color.g;
		colour <<= 8;
		colour += color.b;
		//colour <<= 8;
		//colour += alpha;
		

		auto index = (coord.y * SCREEN_WIDTH) + coord.x;
		auto raw_ptr = screenResources->m_buffer1.get() + index;
		*raw_ptr = colour;
		return ErrorCode::OK;
	}

	void Screen::clear_render()
	{
		screenResources->clear_render();
	}

	void Screen::update_from_pixel_buffer()
	{
		screenResources->update_from_pixel_buffer();
	}

	void Screen::present_render()
	{
		screenResources->present_render();
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

	void Screen::close()
	{
		SDL_Quit();
	}

	void Screen::clear()
	{
		memset(screenResources->m_buffer1.get(), 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
		memset(screenResources->m_buffer2.get(), 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
	}

	void Screen::boxBlur()
	{
		std::swap(screenResources->m_buffer1, screenResources->m_buffer2);

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
							Uint32 colour = screenResources->m_buffer2[currentY * Screen::SCREEN_WIDTH + currentX];

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

				setPixel({ x, y }, 255, { red, green, blue });
			}
		}
	}
}