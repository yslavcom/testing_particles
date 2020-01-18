#pragma once

#include <thread>

class Thread
{
private:
	std::thread t_;

public:
	Thread(std::thread t)
		: t_(std::move(t))
	{}

	~Thread()
	{
		t_.join();
	}
};
