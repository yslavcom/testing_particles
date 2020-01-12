#include<iostream>
#include<vector>
#include<variant>
#include<numeric>

#include "fft.h"
#include "audio.h"
#include "screen.h"
#include "BasicShapesDraw.h"
#include "BasicShapes.h"
#include "DebugLog.h"
#include "Effect.h"
#include "Grid.h"
#include "Axis.h"


using namespace TEST_SCREEN;
using namespace BASIC_SHAPES_2D;

int main(int argc, char* args[])
{
	bool quit = false;

	screen_ptr screen = std::make_shared<Screen>();

	screen->init(1024, 768);
	screen->clear();

	BASIC_EFFECTS::Effect effect;
	Grid grid;
	Axis axis_x(Axis::Type::X, {colour_name::RED});
	Axis axis_y(Axis::Type::Y, { colour_name::RED });

	std::vector<Screen::ScreenWindow> vector_of_scaling_windows;

#if 1
	pixel_vec_2d pixel2d_buf(screen->SCREEN_WIDTH, screen->SCREEN_HEIGHT);

	ScalingWindow window_1{ {0.7f, 0.5f}, 0.19f, 0.45f };
	ScalingWindow window_2{ {0.2f, 0.2f}, 0.3f, 0.32f };
	vector_of_scaling_windows.emplace_back(Screen::to_screen_window(window_1));
	vector_of_scaling_windows.emplace_back(Screen::to_screen_window(window_2));

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	//create graphics
	grid.draw(screen, window_1, pixel2d_buf);
	BasicShapesDraw::draw_dash_line(screen, window_1, pixel2d_buf, pixel_2d_coord_normal{ 0.5, 0.1 }, pixel_2d_coord_normal{ 0.5, 0.9 }, rgb_color_normalized{ colour_name::RED }, 0.1f, 0.01f);
	BasicShapesDraw::draw_dash_line(screen, window_1, pixel2d_buf, pixel_2d_coord_normal{ 0.5, 0.1 }, pixel_2d_coord_normal{ 0.5, 0.9 }, rgb_color_normalized{ colour_name::RED }, 0.1f, 0.01f);
	BasicShapesDraw::draw_dash_line(screen, window_1, pixel2d_buf, pixel_2d_coord_normal{ 0.1, 0.1 }, pixel_2d_coord_normal{ 0.9, 0.9 }, rgb_color_normalized{ colour_name::RED }, 0.1f, 0.01f);

	Screen::ScreenWindow window_screen{ {10, 600}, 100, 200 };
	BasicShapesDraw::draw_dash_line(screen, window_screen, pixel2d_buf, pixel_2d_coord{ 5, 5 }, pixel_2d_coord{ 80, 180 }, rgb_color_normalized{ colour_name::BLUE }, 0.1f, 0.01f);
	BasicShapesDraw::draw_dash_line(screen, window_screen, pixel2d_buf, pixel_2d_coord_normalized{ 0.1, 0.1 }, pixel_2d_coord_normalized{ 0.8, 05 }, rgb_color_normalized{ colour_name::BLUE }, 0.1f, 0.01f);
	grid.draw(screen, window_screen, pixel2d_buf);
	axis_x.draw(screen, window_screen, pixel2d_buf);
	axis_y.draw(screen, window_screen, pixel2d_buf);
	vector_of_scaling_windows.emplace_back(window_screen);

	axis_x.draw(screen, window_2, pixel2d_buf);
	axis_y.draw(screen, window_2, pixel2d_buf);

	SHAPES_2D::Line line(
		screen,
		window_screen,
		pixel_2d_coord_normal{ 0.49, 0.07 }, pixel_2d_coord_normal{ 0.51, 0.91 },
		rgb_color_normalized{1.f, 1.f, 0.f},
		0.1f, 0.01f);

	SHAPES_2D::Line other_line(
		screen,
		window_screen,
		pixel_2d_coord_normal{ 0.09, 0.07 }, pixel_2d_coord_normal{ 0.51, 0.91 },
		rgb_color_normalized{ .5f, 1.f, 0.f },
		0.1f, 0.01f);

	auto window_3 = Screen::ScreenWindow{ {100, 200}, 300, 50 };
	vector_of_scaling_windows.emplace_back(window_3);
	auto baseShape = SHAPES_2D::BaseShape(line);
	auto otherBaseShape = SHAPES_2D::BaseShape(other_line);

	std::vector<SHAPES_2D::BaseShape> vec = { baseShape ,otherBaseShape };
	std::for_each(vec.begin(), vec.end(), 
		[&](auto &el)
		{
			SHAPES_2D::update_window(el, window_3);
			SHAPES_2D::draw(el, pixel2d_buf);
		});

	axis_x.draw(screen, window_3, pixel2d_buf);
	axis_y.draw(screen, window_3, pixel2d_buf);
	

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

