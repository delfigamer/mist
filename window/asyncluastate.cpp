#include "asyncluastate.hpp"
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <utility>

namespace window {
	class LuaError: public utils::StrException {
	public:
		template< typename... Ts >
		LuaError( Ts... args ) :
			utils::StrException( std::forward< Ts... >( args... ) ) {}
	};

	utils::String lua_retrieveerror( lua_State* L ) {
		lua_getglobal( L, "tostring" );
		lua_insert( L, -2 );
		lua_pcall( L, 1, 1, 0 );
		utils::String message( lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
		LOG( "Lua error: %s", message.getchars() );
		return message;
	}

	void AsyncLuaState::threadfunc() noexcept {
		LOG( "~ Lua thread started" );
		lua_State* lstate = luaL_newstate();
		action_t currentaction;
		while( m_running ) {
				if( m_actions.pop( &currentaction ) ) {
					currentaction( lstate, &m_error );
				} else {
					// clock_t time = clock();
					// if( time >= m_time ) {
						m_defaultaction( lstate, &m_error );
						// m_time = time + MinUpdatePeriod;
					// }
				}
			// std::this_thread::yield();
		}
		while( m_actions.pop( &currentaction ) ) {
			currentaction( lstate, &m_error );
		}
		lua_close( lstate );
		LOG( "~ Lua thread finished" );
	}

	AsyncLuaState::AsyncLuaState() :
		m_running( true ) {
		m_thread = std::thread( &AsyncLuaState::threadfunc, this );
		// m_time = clock() + MinUpdatePeriod;
	}

	AsyncLuaState::~AsyncLuaState() noexcept {
	}

	void AsyncLuaState::checkerror() {
		if( m_error ) {
			throw LuaError( std::move( m_error ) );
		}
	}

	void AsyncLuaState::join() {
		m_running = false;
		LOG( "~ Waiting for the Lua thread..." );
		m_thread.join();
		LOG( "~ Lua thread joined" );
	}

	void AsyncLuaState::setdefaultaction(
			AsyncLuaState::action_t const& action ) {
		m_defaultaction = action;
	}

	void AsyncLuaState::schedule(
			AsyncLuaState::action_t const& action ) {
		m_actions.push( action );
	}
}
