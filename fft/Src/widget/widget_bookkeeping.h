#pragma once

#include <shared_mutex>
#include <optional>

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
		std::unique_ptr<Collection<object>>collection;

		WidgetBookeeping()
		{
			collection = std::make_unique<Collection<object>>();
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

			auto count = std::accumulate(collection->iterator_begin()
				, collection->iterator_end()
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

			collection->add(ptr);
			return ptr;
		}

		bool delete_widget(object obj)
		{
			std::unique_lock lk(mut);

			collection->remove(obj);
		}

		std::optional<std::vector<object>> find_windows(pixel_2d_coord&& coord) const
		{
			return do_find_windows(coord);
		}

		std::optional<std::vector<object>> find_windows(const pixel_2d_coord& coord)const
		{
			return do_find_windows(coord);
		}
	};

}
