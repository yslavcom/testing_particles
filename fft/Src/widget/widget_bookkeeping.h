#pragma once

#include <set>
#include <vector>
#include <shared_mutex>
#include <optional>

#include "BasicStructs.h"

namespace BASIC_SHAPES_2D
{
	template <typename T>
	class WidgetBookeeping
	{
		using object = T*;

	private:
		mutable std::shared_mutex mut;
		std::set<object>collection;
		size_t count;

	private:
		inline bool validate(const object w)
		{
			return (nullptr != w);
		}

		WidgetBookeeping()
			: count(0)
		{}
		WidgetBookeeping(const WidgetBookeeping&) = delete;
		WidgetBookeeping(WidgetBookeeping&&) = delete;
		WidgetBookeeping& operator=(const WidgetBookeeping&) = delete;
		WidgetBookeeping& operator = (WidgetBookeeping&&) = delete;

	private:
		template<typename COORD>
		std::optional<std::vector<object>> do_find_windows(COORD&& coord) const
		{
			std::vector<object> result;

			std::shared_lock lk(mut);

			auto count = std::accumulate(collection.begin()
				, collection.end()
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
		static std::shared_ptr<WidgetBookeeping> instance()
		{
			static std::shared_ptr<WidgetBookeeping> inst{ new WidgetBookeeping };
			return inst;
		}

		bool add(const object w)
		{
			std::lock_guard lk(mut);

			if (validate(w))
			{
				collection.insert(w);

				return true;
			}
			else
			{
				return false;
			}
		}

		bool remove(const object w)
		{
			std::lock_guard lk(mut);

			collection.erase(w);

			return true;
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
