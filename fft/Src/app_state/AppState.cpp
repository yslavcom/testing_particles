#include "AppState.h"

namespace APP_STATE
{
	void AppState::register_events()
	{
		events.register_event(Events::EventType::Quit, [&]() {
			DebugLog::instance()->print("quitting...");
			shutting_down();
			});

		events.register_event(Events::EventType::MouseDragging, [&](const pixel_2d_coord& coord) {
			DebugLog::instance()->print("mouse dragged..." + std::to_string(coord.hor) + " " + std::to_string(coord.ver));

			WidgetBookeeping::instance()->move_selected_widgets(coord);
			WidgetBookeeping::instance()->draw_selected_widgets(pixel2d_buf);
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