#include "AppState.h"

namespace APP_STATE
{
	void AppState::register_events()
	{
		auto redraw = [this]()
		{
			try {
				pixel2d_buf.clear();
				WidgetBookeeping::instance()->draw_widgets(pixel2d_buf);
				screen->copy_to_screen_buf(pixel2d_buf);
			}
			catch (const std::exception ex)
			{
				std::cerr << ex.what();
			}
			catch(...)
			{
				std::cerr << "unknown exception";
			}
		};

		events.register_event(Events::EventType::Quit, [&]() {
			DebugLog::instance()->print("quitting...");
			shutting_down();
			});

		events.register_event(Events::EventType::MouseDragging, [&](const pixel_2d_coord& coord) {
			WidgetBookeeping::instance()->move_selected_widgets(coord);

#if 1
			pixel2d_buf.clear();
			WidgetBookeeping::instance()->draw_widgets(pixel2d_buf);
			screen->copy_to_screen_buf(pixel2d_buf);
#endif
			});

		events.register_event(Events::EventType::LeftMouseDown, [&](const pixel_2d_coord& coord) {
			DebugLog::instance()->print("left mouse clicked..." + std::to_string(coord.hor) + " " + std::to_string(coord.ver));
			auto result = WidgetBookeeping::instance()->find_windows(coord);
			if (result.has_value())
			{
				auto selected_widgets = result.value();
				WidgetBookeeping::instance()->add_to_selected(selected_widgets, coord);

				if (selected_widgets.size() == 1) {
					DebugLog::instance()->print("one widget clicked...");
				}
				else {
					DebugLog::instance()->print(std::to_string(selected_widgets.size()) + " widgets clicked...");
				}
			}
			});

		events.register_event(Events::EventType::LeftMouseUp, [&](const pixel_2d_coord& coord) {
			DebugLog::instance()->print("left mouse released..." + std::to_string(coord.hor) + " " + std::to_string(coord.ver));

#if 0
			redraw();
#else
			pixel2d_buf.clear();
			WidgetBookeeping::instance()->draw_widgets(pixel2d_buf);
			screen->copy_to_screen_buf(pixel2d_buf);
#endif
			WidgetBookeeping::instance()->clear_selected_all();
			});


		thread__process_event = std::make_unique<Thread>(std::thread([&] {
				while (is_processing())
				{
					events.process_events();
				}
			})
		);

	}
}