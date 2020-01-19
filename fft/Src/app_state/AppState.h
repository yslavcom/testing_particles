#pragma once

#include <variant>
#include <shared_mutex>
#include <memory>

#include "screen.h"
#include "BasicShapesDraw.h"
#include "BasicShapes.h"
#include "widget_bookkeeping.h"
#include "DebugLog.h"
#include "Effect.h"
#include "Grid.h"
#include "Axis.h"
#include "Line.h"
#include "Events.h"
#include "Thread.h"

namespace APP_STATE
{
	using namespace BASIC_SHAPES_2D;

	namespace
	{
		class start_t
		{};

		class init_resources_t
		{
		};

		class running_t
		{
		};

		class shutting_down_t
		{};

		class shut_t
		{};

		class State
		{
		private:
			mutable std::shared_mutex mut;
			std::variant<start_t, init_resources_t, running_t, shutting_down_t, shut_t> state_;

		public:
			State()
				: state_(start_t())
			{}
			State(const State&) = delete;
			State(const State&&) = delete;
			State operator=(const State&) = delete;
			State operator=(const State&&) = delete;
			virtual ~State() {}

			virtual bool is_starting() const
			{
				std::shared_lock<std::shared_mutex> lk(mut);

				return (std::get_if<start_t>(&state_) != nullptr);
			}

			virtual void init_resources()
			{
				std::lock_guard<std::shared_mutex> lk(mut);
				state_ = init_resources_t();
			}

			virtual bool is_initializing_resources() const
			{
				std::shared_lock<std::shared_mutex> lk(mut);

				return (std::get_if<init_resources_t>(&state_) != nullptr);
			}

			virtual void run()
			{
				std::lock_guard<std::shared_mutex> lk(mut);
				state_ = running_t();
			}

			virtual bool is_running() const
			{
				std::shared_lock<std::shared_mutex> lk(mut);

				return (std::get_if<running_t>(&state_) != nullptr);
			}

			virtual void shutting_down()
			{
				std::lock_guard<std::shared_mutex> lk(mut);
				state_ = shutting_down_t();
			}

			virtual bool is_shutting_down() const
			{
				std::shared_lock<std::shared_mutex> lk(mut);

				return (std::get_if<shutting_down_t>(&state_) != nullptr);
			}

			virtual void shut()
			{
				std::lock_guard<std::shared_mutex> lk(mut);
				state_ = shut_t();
			}

			virtual bool is_shut() const
			{
				std::shared_lock<std::shared_mutex> lk(mut);

				return (std::get_if<shut_t>(&state_) != nullptr);
			}
		};
	}

	class AppState : public State
	{
	public:
		static std::shared_ptr<AppState> instance()
		{
			static std::shared_ptr<AppState> inst{ new AppState };
			return inst;
		}

		bool is_processing()const
		{
			return (!is_shutting_down()
				&& !is_shut());
		}


	private:
		size_t WIDTH;
		size_t HEIGHT;
		screen_ptr screen;
		BASIC_EFFECTS::Effect effect;
		pixel_vec_2d pixel2d_buf;
		
		Events events;

		std::unique_ptr<Thread> thread__process_event;

	private:
		AppState(){
			init_resources();
		}

		void register_events();

	public:

		void init(size_t width, size_t height)
		{
			WIDTH = width;
			HEIGHT = height;
			screen = std::make_shared<Screen>();

			screen->init(WIDTH, HEIGHT);
			screen->clear();

			//create events
			register_events();

			//create graphics
			pixel2d_buf = std::move(pixel_vec_2d{ screen->SCREEN_WIDTH, screen->SCREEN_HEIGHT });

			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			Axis axis_x(Axis::Type::X, { colour_name::RED });
			Axis axis_y(Axis::Type::Y, { colour_name::RED });
			Line line (pixel_2d_coord_normal{ 0.49, 0.07 }, pixel_2d_coord_normal{ 0.51, 0.91 },
				rgb_color_normalized{ 1.f, 1.f, 0.f },
				0.1f, 0.01f);

			auto window_3 = Screen::ScreenWindow{ {100, 200}, 300, 50 };

			Grid grid;

			//test widget
			std::shared_ptr<Widget> widget;
			try
			{
				widget = WidgetBookeeping::instance()->make_widget(ScalingWindow{}, screen);

				widget->add_shape(line);
				widget->add_shape(grid);
				widget->add_shape(axis_x);
				widget->add_shape(axis_y);
				widget->update_window(ScalingWindow{ {0.1f, 0.1f }, 0.3f, 0.4f });
				widget->draw(pixel2d_buf);
			}
			catch (const std::exception & exc)
			{
				std::cerr << exc.what();
			}
			catch (...)
			{
				std::cerr << "unknown exception";
			}

			auto widget2 = WidgetBookeeping::instance()->make_widget(*(widget.get())); // dereference, because 'widget' is a pointer
			widget2->delete_shape_by_index(1);
			pixel_2d_coord new_coord{ 500, 500 };
			widget2->move_window(pixel_2d_coord{ 500, 500 });
			widget2->draw(pixel2d_buf);

			//debug, test pointer
			auto ptr_widget = WidgetBookeeping::instance()->make_widget(Widget(ScalingWindow{}, screen));
			ptr_widget->update_window(ScalingWindow{ {0.2f, 0.5f }, 0.7f, 0.3f });
			ptr_widget->add_shape(grid);
			ptr_widget->draw(pixel2d_buf);

			//WidgetBookeeping::instance()->delete_widget(ptr_widget);

			screen->copy_to_screen_buf(pixel2d_buf);
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			DebugLog::instance()->print("dt = " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + "[ us ]");

		}

		void working_loop()
		{
			while (is_running())
			{
				/*
				the screen functions take about 16 ms in release config,
				it looks like it frame freq depended (?)
				*/
				std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
				screen->clear_render();
				screen->update_from_pixel_buffer();
				screen->present_render();
				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				//DebugLog::instance()->print("dt = " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + "[ us ]");

				events.listen_events();
			}
		}
	};
}
