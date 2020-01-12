#include<iostream>
#include<vector>
#include<variant>
#include<numeric>

#include "fft.h"
#include "audio.h"
#include "screen.h"
#include "BasicShapesDraw.h"
#include "BasicShapes.h"
#include "Widget.h"
#include "DebugLog.h"
#include "Effect.h"
#include "Grid.h"
#include "Axis.h"
#include "Line.h"

using namespace TEST_SCREEN;
using namespace BASIC_SHAPES_2D;

int main(int argc, char* args[])
{
	bool quit = false;

	screen_ptr screen = std::make_shared<Screen>();

	screen->init(1024, 768);
	screen->clear();

	BASIC_EFFECTS::Effect effect;

	

#if 1
	pixel_vec_2d pixel2d_buf(screen->SCREEN_WIDTH, screen->SCREEN_HEIGHT);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	//create graphics
	std::vector<Screen::ScreenWindow> vector_of_scaling_windows;

	Grid grid;
	Axis axis_x(Axis::Type::X, { colour_name::RED });
	Axis axis_y(Axis::Type::Y, { colour_name::RED });

	Line line(
		pixel_2d_coord_normal{ 0.49, 0.07 }, pixel_2d_coord_normal{ 0.51, 0.91 },
		rgb_color_normalized{1.f, 1.f, 0.f},
		0.1f, 0.01f);

	auto window_3 = Screen::ScreenWindow{ {100, 200}, 300, 50 };
	vector_of_scaling_windows.emplace_back(window_3);

	//test widget
	Widget widget(ScalingWindow{}, screen);
	widget.add_shape(line);
	widget.add_shape(grid);
	widget.add_shape(axis_x);
	widget.add_shape(axis_y);
	widget.update_window(ScalingWindow{ {0.1, 0.1 }, 0.3, 0.4 });
	widget.draw(pixel2d_buf);

	//pixel2d_buf.clear();

	Widget widget2(widget);
	widget2.delete_shape_by_index(1);
	pixel_2d_coord new_coord{ 500, 500 };
	//widget2.move_window(new_coord);
	move_widget(widget2, pixel_2d_coord{ 500, 500 });
	widget2.draw(pixel2d_buf);

	screen->copy_to_screen_buf(pixel2d_buf);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	DebugLog::instance()->print("dt = " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + "[ us ]");
#endif

	for (; quit == false;)
	{
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
		}


		auto is_event = screen->processEvents();
		if (is_event.has_value())
		{
			switch (is_event.value().first)
			{
			case Screen::EventType::Quit:
				quit = true;
				DebugLog::instance()->print("quitting...");
				break;

			case Screen::EventType::LeftMouseDown:
			{
				auto click_coord = std::get_if<pixel_2d_coord>(&is_event.value().second);
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
		}
	}

	return 0;
}

