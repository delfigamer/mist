#ifndef WINDOW_ASYNCLUASTATE_HPP__
#define WINDOW_ASYNCLUASTATE_HPP__ 1

#include <utils/mpscqueue.hpp>
#include <utils/configset.hpp>
#include <utils/string.hpp>
#include <utils/delegate.hpp>
#include <utils/console.hpp>
#include <lua/lua.hpp>
#include <atomic>
#include <thread>
#include <exception>
#include <functional>
#include <time.h>

namespace window
{
	utils::String lua_retrieveerror( lua_State* L );

	class AsyncLuaState
	{
	public:
		typedef utils::Delegate<
			void( lua_State* L, utils::String* error ) > action_t;

	private:
		std::thread m_thread;
		std::atomic< bool > m_running;
		action_t m_defaultaction;
		utils::MPSCQueue< action_t > m_actions;
		utils::String m_error;
		std::atomic< uint32_t > m_minupdateperiod;
		uint64_t m_time;
		utils::SingletonRef< utils::ConsoleClass > m_console;

		void threadfunc() noexcept;

	public:
		AsyncLuaState();
		AsyncLuaState( AsyncLuaState const& ) = delete;
		AsyncLuaState( AsyncLuaState&& ) = delete;
		~AsyncLuaState() noexcept;
		AsyncLuaState& operator=( AsyncLuaState const& ) = delete;
		AsyncLuaState& operator=( AsyncLuaState&& ) = delete;

		void applyconfig( utils::ConfigSet const& config );
		void checkerror();
		void join();
		void setdefaultaction( action_t const& action );
		void schedule( action_t const& action );

		template< typename... Ts >
		void setdefaultaction( Ts... args )
		{
			setdefaultaction( action_t( std::forward< Ts >( args )... ) );
		}

		template< typename... Ts >
		void schedule( Ts... args )
		{
			schedule( action_t( std::forward< Ts >( args )... ) );
		}
	};
}

#endif
