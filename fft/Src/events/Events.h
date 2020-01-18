#pragma once

#include <SDL.h>

#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <variant>
#include <optional>

#include "Pipe.h"
#include "BasicStructs.h"

using namespace BASIC_SHAPES_2D;

class Events
{
public:

	//template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	//template<class... Ts> overload(Ts...)->overload<Ts...>;


	using pPlainEventFunc = std::function<void(void)>;
	using pCoordEventFunc = std::function<void(const pixel_2d_coord& )>;

	using foo_container = std::variant<pPlainEventFunc, pCoordEventFunc>;

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
	std::map<EventType, foo_container> mapEvents;

private:
	template<typename T, typename V>
	auto get_if(V& v)->std::optional<T>
	{
		auto ptr = std::get_if<T>(&v);

		if (nullptr == ptr) return{};
		else return { *ptr };
	}

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
		mapEvents = std::move(other.mapEvents);
	}
	Events& operator=(Events&& other)
	{
		std::lock_guard<std::mutex> lk(mut);

		this->pipe = std::move(other.pipe);
		this->mapEvents = std::move(other.mapEvents);
		return *this;
	}
	Events& operator=(const Events& other) = delete;

	template <typename FOO>
	void register_event(EventType eventType, FOO&& fn)
	{
		mapEvents[eventType] = fn;
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
		auto event_pair_ptr = pipe->wait_and_pop();

		if (nullptr != event_pair_ptr) // ok, we have an event
		{
			auto foo_container = mapEvents.find(event_pair_ptr->first); // get the variant with functions
			if (foo_container != mapEvents.end())
			{
				auto fn_opt = get_if<pCoordEventFunc>(foo_container->second); // optionally get the function
				if (fn_opt.has_value())
				{
					auto params_opt = get_if<pixel_2d_coord, EventContainer>(event_pair_ptr->second); // opionally get the function params
					if (params_opt.has_value())
					{
						auto fn = fn_opt.value();
						auto params = params_opt.value();
						fn(params);
					}
				}
				else
				{
					auto fn_opt = get_if<pPlainEventFunc>(foo_container->second); // optionally get the function
					if (fn_opt.has_value())
					{
						auto fn = fn_opt.value();
						fn();
					}
				}
			}
		}
	}

};
