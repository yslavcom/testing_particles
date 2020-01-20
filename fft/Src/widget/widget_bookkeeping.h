#pragma once

#include <shared_mutex>
#include <optional>
#include <numeric>
#include <utility>
#include <memory>

#include "BasicStructs.h"
#include "collection.h"
#include "Widget.h"

namespace BASIC_SHAPES_2D
{
	class WidgetBookeeping
	{
		using Object = std::shared_ptr<Widget>;
		using Coord = pixel_2d_coord::delta;
		using Object_and_coord_pair = std::pair<Object, Coord>;

	private:
		mutable std::shared_mutex mut;
		std::unique_ptr<Collection<Object>>widgets_col;
		std::unique_ptr<Collection<Object_and_coord_pair>>widgets_selected;

		WidgetBookeeping()
		{
			widgets_col = std::make_unique<Collection<Object>>();
			widgets_selected = std::make_unique<Collection<Object_and_coord_pair>>();
		}

		WidgetBookeeping(const WidgetBookeeping&) = delete;
		WidgetBookeeping(WidgetBookeeping&&) = delete;
		WidgetBookeeping& operator=(const WidgetBookeeping&) = delete;
		WidgetBookeeping& operator = (WidgetBookeeping&&) = delete;

	public:
		static std::shared_ptr<WidgetBookeeping> instance()
		{
			static std::shared_ptr<WidgetBookeeping> inst{
				new WidgetBookeeping
			};
			return inst;
		}

	private:
		template<typename COORD>
		std::optional<std::vector<Object>> do_find_windows(COORD&& coord) const
		{
			std::vector<Object> result;

			std::shared_lock lk(mut);

			auto count = std::accumulate(widgets_col->iterator_begin()
				, widgets_col->iterator_end()
				, 0
				, [&](auto& count, auto& w)->int
				{
					auto window = w->get_window();
					if (window.inside(coord))
					{
						result.push_back(w);
						count++;
					}
					return count;
				});

			if (count)return result;
			else return {};
		}

	public:

		template<typename ... Args>
		Object make_widget(Args...args)
		{
			std::unique_lock lk(mut);

			Object ptr(new Widget(std::forward<Args>(args)...));

			if (!ptr) { throw std::system_error(errno, std::generic_category()); }

			widgets_col->add(ptr);
			return ptr;
		}

		bool delete_widget(Object obj)
		{
			std::unique_lock lk(mut);

			return widgets_col->remove(obj);
		}

		std::optional<std::vector<Object>> find_windows(pixel_2d_coord&& coord) const
		{
			return do_find_windows(coord);
		}

		std::optional<std::vector<Object>> find_windows(const pixel_2d_coord& coord)const
		{
			return do_find_windows(coord);
		}

		template<typename COL>
		void move_widgets(COL&& col, const Coord& coord)
		{
			std::for_each(
				col.begin(),
				col.end(),
				[](auto& widget)
				{
					auto window = widget->get_window();

					widget->move_window(coord);
				}
			);
		}

		template<typename COL>
		void draw_widgets(COL&& col, pixel_vec_2d& pixel2d_buf)
		{
			std::for_each(
				col.begin(),
				col.end(),
				[](auto& widget)
				{
					widget->draw(pixel2d_buf);
				}
			);
		}

		template<typename COL>
		void add_to_selected(COL&& col, const pixel_2d_coord& coord)
		{
			std::unique_lock lk(mut);

			std::for_each(
				col.begin(),
				col.end(),
				[&](auto widget)
				{
					pixel_2d_coord w_coord = widget->get_window().corner_coord;
					auto delta_coord = w_coord.get_delta(coord);
					widgets_selected->add(std::make_pair(widget, delta_coord));
				}
			);
		}

		template<typename COL>
		void clear_selected(COL&& col)
		{
			std::unique_lock lk(mut);

			std::for_each(
				col.begin(),
				col.end(),
				[](auto& widget)
				{
					std::find_if(
						widgets_selected->iterator_begin(),
						widgets_selected->iterator_end(),
						[](auto& el) {
							if (std::get<Object>(el) == widget)
							{
								widgets_selected->remove(el);
								return true;
							}
							else
							{
								return false;
							}
						}
					);
				}
			);
		}

		void clear_selected_all()
		{
			std::unique_lock lk(mut);

			widgets_selected->clear_all();
		}

		std::optional<std::vector<Object>> get_selected()const
		{
			std::shared_lock lk(mut);

			if (0 == widgets_selected->get_count())return {};

			std::vector<Object> result;

			std::for_each(
				widgets_selected->iterator_begin(),
				widgets_selected->iterator_end(),
				[&](auto& el)
				{
					result.emplace_back(std::get<Object>(el));
				}
			);

			return result;
		}

		void move_selected_widgets(const pixel_2d_coord& coord)
		{
			std::for_each(
				widgets_selected->iterator_begin(),
				widgets_selected->iterator_end(),
				[&](auto& el)
				{
					Coord delta = std::get<Coord>(el);
					auto widget = std::get<Object>(el);
					auto window = widget->get_window();
					auto corner_coord = window.corner_coord;
					corner_coord.apply_delta(delta);

					DebugLog::instance()->print("coord " + std::to_string(coord.hor) + "  " + std::to_string(corner_coord.hor));

					widget->move_window(coord);
				}
			);
		}

		void draw_selected_widgets(pixel_vec_2d& pixel2d_buf)
		{
			std::for_each(
				widgets_selected->iterator_begin(),
				widgets_selected->iterator_end(),
				[&](auto& el)
				{
					auto widget = std::get<Object>(el);
					widget->draw(pixel2d_buf);
				}
			);
		}
	};

}
