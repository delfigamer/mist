#ifndef UTILS_LUA_HPP__
#define UTILS_LUA_HPP__

#include "string.hpp"
#include <lua/lua.hpp>
#include <exception>

namespace utils {
	class LuaState {
	public:
		class Exception: public std::exception {
		private:
			String m_msg;
			
		public:
			Exception() = delete;
			Exception( lua_State* L );
			Exception( Exception const& other );
			Exception( Exception&& other );
			virtual ~Exception() override;
			Exception& operator=( Exception const& other );
			Exception& operator=( Exception&& other );
			virtual char const* what() const noexcept override;
		};
	private:
		lua_State* m_state;
		static int atpanic( lua_State* L );
		
	public:
		LuaState();
		LuaState( lua_State* state );
		LuaState( LuaState const& other ) = delete;
		LuaState( LuaState&& other );
		~LuaState();
		LuaState& operator=( LuaState const& other ) = delete;
		LuaState& operator=( LuaState&& other );
		
		operator lua_State*() const;
		
		int ploadfile( String const& path );
		void loadfile( String const& path );
		int ploadstring( String const& str, String const& name = 0 );
		void loadstring( String const& str, String const& name = 0 );
		int pcall( int an, int rn, int ef = 0 );
		void call( int an, int rn, int ef = 0 );
		void pushnil();
		void pushboolean( bool b );
		void pushinteger( int i );
		void pushnumber( double n );
		void pushlightuserdata( void* ud );
		void pushstring( String const& str );
		void pushvalue( int idx );
		int gettop();
		void settop( int nv );
		void error();
	};
}

#endif
