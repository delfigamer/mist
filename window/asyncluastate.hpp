#ifndef WINDOW_ASYNCLUASTATE_HPP__
#define WINDOW_ASYNCLUASTATE_HPP__ 1

#include <utils/mpscqueue.hpp>
#include <utils/string.hpp>
#include <utils/method.hpp>
#include <lua/lua.hpp>
#include <atomic>
#include <thread>
#include <exception>
#include <functional>
#include <ctime>

namespace window {
	utils::String lua_retrieveerror( lua_State* L );

	class AsyncLuaState {
	public:
		typedef utils::Method<
			void( lua_State* L, utils::String* error ) > action_t;

	private:
		enum {
			MinUpdatePeriod = CLOCKS_PER_SEC / 100,
		};
		std::thread m_thread;
		std::atomic< bool > m_running;
		action_t m_defaultaction;
		utils::MPSCQueue< action_t > m_actions;
		utils::String m_error;
		clock_t m_time;

		void threadfunc() noexcept;

	public:
		AsyncLuaState();
		AsyncLuaState( AsyncLuaState const& ) = delete;
		AsyncLuaState( AsyncLuaState&& ) = delete;
		~AsyncLuaState() noexcept;
		AsyncLuaState& operator=( AsyncLuaState const& ) = delete;
		AsyncLuaState& operator=( AsyncLuaState&& ) = delete;

//		bool isbusy() noexcept;
		void checkerror();
		void join();
		void setdefaultaction( action_t const& action );
		void schedule( action_t const& action );

		template< typename... Ts >
		void setdefaultaction( Ts... args ) {
			setdefaultaction(
				action_t( std::forward< Ts >( args )... ) );
		}

		template< typename... Ts >
		void schedule( Ts... args ) {
			schedule( action_t( std::forward< Ts >( args )... ) );
		}
	};
}

#endif
