#include "lua.hpp"
#include "console.hpp"
#include <utility>
#include <cstddef>

namespace utils {
	void logstack( lua_State* L ) {
		int ssize = lua_gettop( L );
		for( int i = 1; i <= ssize; i++ ) {
			MainLogger().writeln( "%i: (%s) %s", i, luaL_typename( L, i ), "" );
		}
	}

	LuaState::Exception::Exception( lua_State* L ) {
		size_t length;
		lua_getglobal( L, "tostring" );
		lua_insert( L, -2 );
		lua_pcall( L, 1, 1, 0 );
		char const* chars = lua_tolstring( L, -1, &length );
		m_msg.chars( chars, length );
		lua_pop( L, 1 );
	}

	LuaState::Exception::Exception( LuaState::Exception const& other ) :
		m_msg( other.m_msg ) {
	}

	LuaState::Exception::Exception( LuaState::Exception&& other ) :
		m_msg( std::move( other.m_msg ) ) {
	}

	LuaState::Exception::~Exception() {
	}

	LuaState::Exception& LuaState::Exception::operator=( LuaState::Exception const& other ) {
		m_msg = other.m_msg;
		return *this;
	}

	LuaState::Exception& LuaState::Exception::operator=( LuaState::Exception&& other ) {
		m_msg = std::move( other.m_msg );
		return *this;
	}

	char const* LuaState::Exception::what() const noexcept {
		return m_msg;
	}

	int LuaState::atpanic( lua_State* L ) {
		LOG( "lua state panic: %s", lua_tostring( L, -1 ) );
		std::terminate();
	}

	LuaState::LuaState() :
		m_state( luaL_newstate() ) {
		lua_atpanic( m_state, &LuaState::atpanic );
	}

	LuaState::LuaState( lua_State* state ) :
		m_state( state ) {
	}

	LuaState::LuaState( LuaState&& other ) :
		m_state( other.m_state ) {
		other.m_state = 0;
	}

	LuaState::~LuaState() {
		if( m_state ) {
			lua_close( m_state );
		}
	}

	LuaState& LuaState::operator=( LuaState&& other ) {
		m_state = other.m_state;
		other.m_state = 0;
		return *this;
	}

	LuaState::operator lua_State*() const {
		return m_state;
	}

	int LuaState::ploadfile( String const& path ) {
		return luaL_loadfile( m_state, path );
	}

	void LuaState::loadfile( String const& path ) {
		if( ploadfile( path ) != 0 ) {
			error();
		}
	}

	int LuaState::ploadstring( String const& str, String const& name ) {
		return luaL_loadbuffer( m_state, str, str.length(), name ? name : str );
	}

	void LuaState::loadstring( String const& str, String const& name ) {
		if( ploadstring( str, name ) != 0 ) {
			error();
		}
	}

	int LuaState::pcall( int an, int rn, int ef ) {
		return lua_pcall( m_state, an, rn, ef );
	}

	void LuaState::call( int an, int rn, int ef ) {
		if( pcall( an, rn, ef ) != 0 ) {
			error();
		}
	}

	void LuaState::pushnil() {
		lua_pushnil( m_state );
	}

	void LuaState::pushboolean( bool b ) {
		lua_pushboolean( m_state, b );
	}

	void LuaState::pushinteger( int i ) {
		lua_pushinteger( m_state, i );
	}

	void LuaState::pushnumber( double n ) {
		lua_pushnumber( m_state, n );
	}

	void LuaState::pushlightuserdata( void* ud ) {
		lua_pushlightuserdata( m_state, ud );
	}

	void LuaState::pushstring( String const& str ) {
		lua_pushlstring( m_state, str.chars(), str.length() );
	}

	void LuaState::pushvalue( int idx ) {
		lua_pushvalue( m_state, idx );
	}

	int LuaState::gettop() {
		return lua_gettop( m_state );
	}

	void LuaState::settop( int nv ) {
		lua_settop( m_state, nv );
	}

	void LuaState::error() {
		throw Exception( m_state );
	}
}
