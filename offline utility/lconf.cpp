#include <lua/lua.hpp>
#include <string>
#include <cstdio>

void config_loadfile( lua_State* L, char const* path )
{
	if( luaL_loadfile( L, "conf.lua" ) || lua_pcall( L, 0, 0, 0 ) )
	{
		fprintf( stderr, "error in config file \"%s\": %s\n",
			path, lua_tostring( L, -1 ) );
		std::terminate();
	}
}

void lua_getvalue( lua_State* L, int index, bool* result )
{
	*result = lua_toboolean( L, index );
}

void lua_getvalue( lua_State* L, int index, int* result )
{
	*result = lua_tointeger( L, index );
}

void lua_getvalue( lua_State* L, int index, double* result )
{
	*result = lua_tonumber( L, index );
}

void lua_getvalue( lua_State* L, int index, std::string* result )
{
	size_t len;
	char const* ch = lua_tolstring( L, index, &len );
	*result = std::string( ch, ch + len );
}

template< typename T >
T config_getvalue(
	lua_State* L, std::string const& expression, T const& defval )
{
	T result;
	std::string chunk = std::string( "return " ) + expression;
	if( luaL_loadstring( L, chunk.c_str() ) || lua_pcall( L, 0, 1, 0 ) ||
		lua_isnil( L, -1 ) )
	{
		result = defval;
	}
	else
	{
		lua_getvalue( L, -1, &result );
	}
	lua_pop( L, -1 );
	return result;
}

int main( int argc, char const** argv )
{
	char const* platform = argc >= 2 ? argv[ 1 ] : "default";
	lua_State* conf = luaL_newstate();
	lua_pushstring( conf, platform );
	lua_setglobal( conf, "platform" );
	config_loadfile( conf, "conf.lua" );
	int windowwidth = config_getvalue( conf, "display.width", 640 );
	int windowheight = config_getvalue( conf, "display.height", 480 );
	float gamma = config_getvalue( conf, "display.gamma", 2.2 );
	bool fullscreen = config_getvalue( conf, "display.fullscreen", false );
	std::string renderername = config_getvalue(
		conf, "display.renderer", std::string( "d3d9renderer" ) );
	printf( "window size = %i*%i\n", windowwidth, windowheight );
	printf( "gamma = %.5f\n", gamma );
	printf( "fullscreen = %s\n", fullscreen ? "true" : "false" );
	printf( "renderer library = %s.dll\n", renderername.c_str() );
	return 0;
}
