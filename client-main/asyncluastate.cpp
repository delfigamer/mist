#include "asyncluastate.hpp"
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <utility>

namespace window
{
	class LuaError: public utils::StrException
	{
	public:
		template< typename... Ts >
		LuaError( Ts... args )
			: utils::StrException( std::forward< Ts... >( args... ) )
		{
		}
	};

	utils::String lua_retrieveerror( lua_State* L )
	{
		lua_getglobal( L, "tostring" );
		lua_insert( L, -2 );
		lua_pcall( L, 1, 1, 0 );
		utils::String message( lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
		return message;
	}

	static uint64_t gettime()
	{
		timespec ts;
		clock_gettime( CLOCK_MONOTONIC, &ts );
		return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
	}

	void AsyncLuaState::threadfunc() noexcept
	{
		LOG( "~ Lua thread started" );
		lua_State* lstate = luaL_newstate();
		action_t currentaction;
		while( m_running )
		{
				if( m_actions.pop( &currentaction ) )
				{
					currentaction( lstate, &m_error );
				}
				else
				{
					uint32_t minupdateperiod =
						m_minupdateperiod.load( std::memory_order_relaxed );
					if( minupdateperiod != 0 )
					{
						uint64_t time = gettime();
						if( time < m_time )
						{
							std::this_thread::yield();
							continue;
						}
						m_time = time + minupdateperiod;
					}
					m_defaultaction( lstate, &m_error );
				}
		}
		while( m_actions.pop( &currentaction ) )
		{
			currentaction( lstate, &m_error );
		}
		lua_close( lstate );
		LOG( "~ Lua thread finished" );
	}

	AsyncLuaState::AsyncLuaState()
		: m_running( true )
		, m_minupdateperiod( 0 )
		, m_time( 0 )
	{
		m_thread = std::thread( &AsyncLuaState::threadfunc, this );
	}

	AsyncLuaState::~AsyncLuaState() noexcept
	{
	}

	void AsyncLuaState::applyconfig( utils::ConfigSet const& config )
	{
		int tpslimit = config.integer( "tpslimit", 1000 );
		if( tpslimit != 0 )
		{
			m_minupdateperiod.store(
				1000000000 / tpslimit,
				std::memory_order_relaxed );
		}
		else
		{
			m_minupdateperiod.store( 0, std::memory_order_relaxed );
		}
	}

	void AsyncLuaState::checkerror()
	{
		if( m_error )
		{
			throw LuaError( std::move( m_error ) );
		}
	}

	void AsyncLuaState::join()
	{
		m_running = false;
		LOG( "~ Waiting for the Lua thread..." );
		m_thread.join();
		LOG( "~ Lua thread joined" );
	}

	void AsyncLuaState::setdefaultaction( AsyncLuaState::action_t const& action )
	{
		m_defaultaction = action;
	}

	void AsyncLuaState::schedule( AsyncLuaState::action_t const& action )
	{
		m_actions.push( action );
	}
}
