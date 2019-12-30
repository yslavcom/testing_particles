#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <memory>

class DebugLog
{
private:
	std::mutex mutex;

public:
	DebugLog() {}
	DebugLog(const DebugLog&) = delete;
	DebugLog(const DebugLog&&) = delete;
	DebugLog operator=(const DebugLog&) = delete;
	DebugLog operator=(const DebugLog&&) = delete;


	static std::shared_ptr<DebugLog> instance()
	{
		static std::shared_ptr<DebugLog> s{ new DebugLog };
		return s;
	}

	void print(const std::string&& str)
	{
		std::lock_guard<std::mutex> lk(mutex);
		std::cout << str << std::endl;
	}
};
