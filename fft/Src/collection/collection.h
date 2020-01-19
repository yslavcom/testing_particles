#pragma once

#include <set>

template <typename T>
class Collection
{

private:
	std::set<T> collection;

public:
	Collection()
	{
		collection.clear();
	}

	Collection(const Collection& other)
	{
		collection = other.collection;
	}

	Collection(Collection&& other)
		: collection(std::move(other.collection))
	{
		
	}

	Collection& operator=(const Collection& other)
	{
		collection = other.collection;

		return *this;
	}

	Collection& operator = (Collection&& other)
	{
		collection(std::move(other.collection));

		return *this;
	}


public:
	bool add(const T obj)
	{
		collection.emplace(obj);

		return true;
	}

	bool remove(const T obj)
	{
		collection.erase(obj);

		return true;
	}

	bool clear_all()
	{
		collection.clear();

		return true;
	}

	auto iterator_begin()const
	{
		return collection.begin();
	}

	auto iterator_end()const
	{
		return collection.end();
	}

	size_t get_count()const
	{
		return collection.size();
	}
};
