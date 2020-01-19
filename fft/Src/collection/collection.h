#pragma once

#include <set>

template <typename T>
class Collection
{
private:
	std::set<T>collection;
	size_t count;

public:
	Collection()
		: count(0)
	{
		collection.clear();
	}

	Collection(const Collection& other)
	{
		collection = other.collection;
		count = other.count;
	}

	Collection(Collection&& other)
	{
		collection(std::move(other.collection));
		count(std::move(other.count));
	}

	Collection& operator=(const Collection& other)
	{
		collection = other.collection;
		count = other.count;

		return *this;
	}

	Collection& operator = (Collection&& other)
	{
		collection(std::move(other.collection));
		count(std::move(other.count));

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

	auto iterator_begin()
	{
		return collection.begin();
	}

	auto iterator_end()
	{
		return collection.end();
	}
};
