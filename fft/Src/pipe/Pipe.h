#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

template<typename T>
class ThreadsafePipe
{
private:
	std::queue<T> pipe_;

	mutable std::mutex mut;
	std::condition_variable data_cond;

public:
	ThreadsafePipe() {};
	ThreadsafePipe(const ThreadsafePipe& other) = delete;
	ThreadsafePipe(ThreadsafePipe&& other) = delete;
	ThreadsafePipe& operator=(const ThreadsafePipe& other) = delete;
	ThreadsafePipe& operator=(ThreadsafePipe&& other) = delete;

	template<typename T>
	void push(T&& new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		pipe_.emplace(new_value);
		data_cond.notify_one();
	}

	void wait_and_pop(T& val)
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this]() {return !pipe_.empty(); });
		val = pipe_.front();
		pipe_.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this]() {return !pipe_.empty(); });
		auto result(std::make_shared<T>(pipe_.front()));
		pipe_.pop();

		return result;
	}
	
	bool try_pop(T& val)
	{
		std::lock_guard<std::mutex> lk(mut);

		if (pipe_.empty())return false;

		val = pipe_.front();
		pipe_.pop();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mut);

		if (pipe_.empty())return nullptr;

		auto result(std::make_shared<T>(pipe_.front()));
		pipe_.pop();

		return result;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);

		return pipe_.empty();
	}
};