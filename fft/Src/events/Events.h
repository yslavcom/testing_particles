#pragma once

#include <SDL.h>

#include <memory>
#include <functional>
#include <mutex>
#include <map>

#include "Pipe.h"
#include "BasicStructs.h"

using namespace BASIC_SHAPES_2D;

class Events
{
public:
	using pEventFunc = std::function<void(void)>;

	enum  class EventType
	{
		LeftMouseDown, // left mouse button pressed
		MouseDragging, // left mouse button down & mouse is moving
		Quit, // quit app event
	};

	using EventContainer = std::variant<pixel_2d_coord, std::monostate>;
	using event_pair_t = std::pair<EventType, EventContainer>;

private:
	std::mutex mut;
	std::unique_ptr<ThreadsafePipe<event_pair_t>> pipe;
	std::map<EventType, pEventFunc> registeredEvents;

public:

	virtual ~Events() {}

	Events() 
	{
		pipe = std::make_unique<ThreadsafePipe<event_pair_t>>();
	}

	Events(const Events& other) = delete;
	Events(Events&& other)
	{
		std::lock_guard<std::mutex> lk(mut);

		pipe = std::move(other.pipe);
		registeredEvents = std::move(other.registeredEvents);
	}
	Events& operator=(Events&& other)
	{
		std::lock_guard<std::mutex> lk(mut);

		this->pipe = std::move(other.pipe);
		this->registeredEvents = std::move(other.registeredEvents);
		return *this;
	}
	Events& operator=(const Events& other) = delete;

	void register_event(EventType eventType, pEventFunc fn)
	{
		registeredEvents[eventType] = fn;
	}

	/*
	This method must be called from one single thread only !
	*/
	void listen_events()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
			{
				pipe->push(std::make_pair<EventType, EventContainer>(EventType::Quit, std::monostate()));
			}break;

			case SDL_MOUSEBUTTONDOWN:
			{
				if (SDL_BUTTON(SDL_BUTTON_LEFT) == event.button.button)
				{
					pixel_2d_coord coord;
					coord.hor = event.button.x;
					coord.ver = event.button.y;
					pipe->push(std::make_pair<EventType, EventContainer>(EventType::LeftMouseDown, coord));
				}
			}break;

			case SDL_MOUSEMOTION:
			{
				if (SDL_BUTTON_LMASK & event.motion.state)
				{
					pixel_2d_coord coord;
					coord.hor = event.motion.x;
					coord.ver = event.motion.y;
					pipe->push(std::make_pair<EventType, EventContainer>(EventType::MouseDragging, coord));
				}
			}
			}break;

		}
	}

	void process_events()
	{
		std::lock_guard<std::mutex> lk(mut);

		event_pair_t is_event;
		if (pipe->try_pop(is_event))
		{
			auto search = registeredEvents.find(is_event.first);
			if (search != registeredEvents.end())
			{
				if (search->second)
				{
					auto fn = search->second;
					fn();
				}
			}
#if 0
			switch (is_event.first)
			{
			case EventType::Quit:
				break;

			case EventType::LeftMouseDown:
			{
				auto click_coord = std::get_if<pixel_2d_coord>(&is_event.second);
				if (nullptr != click_coord)
				{
					std::accumulate(vector_of_scaling_windows.begin()
						, vector_of_scaling_windows.end()
						, 0
						, [=](int index, auto& w)->int
						{
							if (click_coord->hor >= w.get_vertex_coord(Screen::ScreenWindow::Vertex::A).hor
								&& click_coord->hor <= w.get_vertex_coord(Screen::ScreenWindow::Vertex::B).hor)
							{
								if (click_coord->ver >= w.get_vertex_coord(Screen::ScreenWindow::Vertex::C).ver
									&& click_coord->ver <= w.get_vertex_coord(Screen::ScreenWindow::Vertex::A).ver)
								{
									DebugLog::instance()->print("window # " + std::to_string(index) + " clicked");
								}
							}
							return index + 1;
						});
				}
			}
			break;
			}
#endif
		}

	}

};
