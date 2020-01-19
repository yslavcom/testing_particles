#pragma once

#include <shared_mutex>
#include <optional>
#include <numeric>
#include <utility>

#include "BasicStructs.h"
#include "collection.h"
#include "Widget.h"

namespace BASIC_SHAPES_2D
{
	class WidgetBookeeping
	{
		using object = std::shared_ptr<Widget>;

	private:
		mutable std::shared_mutex mut;
		std::unique_ptr<Collection<object>>widgets_col;
		std::unique_ptr<Collection<object>>widgets_selected;

		WidgetBookeeping()
		{
			widgets_col = std::make_unique<Collection<object>>();
			widgets_selected = std::make_unique<Collection<object>>();
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
		std::optional<std::vector<object>> do_find_windows(COORD&& coord) const
		{
			std::vector<object> result;

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
		object make_widget(Args...args)
		{
			std::unique_lock lk(mut);

			object ptr(new Widget(std::forward<Args>(args)...));

			if (!ptr) { throw std::system_error(errno, std::generic_category()); }

			widgets_col->add(ptr);
			return ptr;
		}

		bool delete_widget(object obj)
		{
			std::unique_lock lk(mut);

			return widgets_col->remove(obj);
		}

		std::optional<std::vector<object>> find_windows(pixel_2d_coord&& coord) const
		{
			return do_find_windows(coord);
		}

		std::optional<std::vector<object>> find_windows(const pixel_2d_coord& coord)const
		{
			return do_find_windows(coord);
		}

		template<typename COL>
		void move_widgets(COL&& col, const pixel_2d_coord& coord)
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
					widgets_selected->add(widget);
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
#if 1
#else
					std::find_if(
						widgets_selected->iterator_begin(),
						widgets_selected->iterator_end(),
						[](auto& el) {
							if (std::get<object>(el) == widget)
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
#endif
				}
			);
		}

		void clear_selected_all()
		{
			std::unique_lock lk(mut);

			widgets_selected->clear_all();
		}

		std::optional<std::vector<object>> get_selected()const
		{
			std::shared_lock lk(mut);

			if (0 == widgets_selected->get_count())return {};

			std::vector<object> result;

			std::for_each(
				widgets_selected->iterator_begin(),
				widgets_selected->iterator_end(),
				[&](auto& el)
				{
					//result.emplace_back(std::get<object>(el));
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
					//auto widget = std::get<object>(el);
					//widget->move_window(coord);
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
					//auto widget = std::get<object>(el);
					//widget->draw(pixel2d_buf);
				}
			);
		}
	};

}
