#include <lua/lua.hpp>
// #include <common.hpp>
// #include <osapi.hpp>
#include <cstdio>

int threadf( lua_State* L )
{
	int* ptr = ( int* )lua_touserdata( L, lua_upvalueindex( 1 ) );
	printf( "threadf 1\ti = %i\n", *ptr );
	lua_yield( L, 0 );
	printf( "threadf 2\ti = %i\n", *ptr );
	*ptr = 2;
	lua_yield( L, 0 );
	printf( "threadf 3\ti = %i\n", *ptr );
	*ptr = 3;
	return 0;
}

int main( int argc, char const* const* argv )
{
	lua_State* L = luaL_newstate();
	lua_State* T = lua_newthread( L );
	int i;
	int ret;
	lua_pushlightuserdata( T, &i );
	lua_pushcclosure( T, &threadf, 1 );
	i = -1;
	ret = lua_resume( T, 0 );
	printf( "main 1\ti = %i\tret = %i\n", i, ret );
	i = -2;
	ret = lua_resume( T, 0 );
	printf( "main 2\ti = %i\tret = %i\n", i, ret );
	i = -3;
	ret = lua_resume( T, 0 );
	printf( "main 3\ti = %i\tret = %i\n", i, ret );
	return 0;
}
