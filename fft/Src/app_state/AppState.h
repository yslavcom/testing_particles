#pragma once

#include <variant>
#include <shared_mutex>
#include <memory>

namespace APP_STATE
{
	namespace
	{
		class start_t
		{};

		class init_resources_t
		{};

		class running_t
		{};

		class shutting_down_t
		{};

		class shut_t
		{};
	}

	class AppState
	{
	private:
		mutable std::shared_mutex mut;
		std::variant<start_t, init_resources_t, running_t, shutting_down_t, shut_t> state_;

	public:
		AppState()
			: state_(start_t())
		{}
		AppState(const AppState&) = delete;
		AppState(const AppState&&) = delete;
		AppState operator=(const DebugLog&) = delete;
		AppState operator=(const DebugLog&&) = delete;

		static std::shared_ptr<AppState> instance()
		{
			static std::shared_ptr<AppState> inst{ new AppState };
			return inst;
		}

		bool is_starting() const
		{
			std::shared_lock<std::shared_mutex> lk(mut);

			return (std::get_if<start_t>(&state_) != nullptr);
		}

		void init_resources()
		{
			std::lock_guard<std::shared_mutex> lk(mut);
			state_ = init_resources_t();
		}

		bool is_initializing_resources() const
		{
			std::shared_lock<std::shared_mutex> lk(mut);

			return (std::get_if<init_resources_t>(&state_) != nullptr);
		}

		void run()
		{
			std::lock_guard<std::shared_mutex> lk(mut);
			state_ = running_t();
		}

		bool is_running() const
		{
			std::shared_lock<std::shared_mutex> lk(mut);
			
			return (std::get_if<running_t>(&state_) != nullptr);
		}

		void shutting_down()
		{
			std::lock_guard<std::shared_mutex> lk(mut);
			state_ = shutting_down_t();
		}

		bool is_shutting_down() const
		{
			std::shared_lock<std::shared_mutex> lk(mut);

			return (std::get_if<shutting_down_t>(&state_) != nullptr);
		}

		void shut()
		{
			std::lock_guard<std::shared_mutex> lk(mut);
			state_ = shut_t();
		}

		bool is_shut() const
		{
			std::shared_lock<std::shared_mutex> lk(mut);

			return (std::get_if<shut_t>(&state_) != nullptr);
		}

		bool is_processing()const
		{
			return (!is_shutting_down()
				&& !is_shut());
		}
	};
}
