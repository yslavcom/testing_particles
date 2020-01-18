#pragma once

#include <set>
#include <shared_mutex>
#include <optional>

namespace BASIC_SHAPES_2D
{
	template <typename T>
	class WidgetBookeeping
	{
		using object = T*;

	private:
		std::shared_mutex mut;
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

		std::optional<object> find(const object w)
		{
			std::shared_lock lk(mut);

			auto found = collection.find(w);
			if (found != collection.end())
			{
				return *found;
			}
			else
			{
				return {};
			}
		}
	};

}
