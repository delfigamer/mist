#include <lua/lua.hpp>
#include <cstdio>
#include <cstring>

int main( int argc, char const** argv )
{
	if( argc < 3 )
	{
		fprintf( stderr, "Syntax: luac <target> <source> [<flags>]\n" );
		fprintf( stderr, "Possible flags:\n" );
		fprintf( stderr, "\td  remove debug information\n" );
		return EXIT_ERROR;
	}
	bool nodebug = false;
	if( argc >= 4 )
	{
		nodebug = strchr( argv[ 3 ], 'd' );
	}
	lua_State* L = luaL_newstate();
	luaL_openlibs( L );
	if( luaL_loadfile( L, argv[ 2 ] ) != 0 )
	{
		fprintf( stderr,
			"Failed to load source file: %s\n", lua_tostring( L, -1 ) );
		return EXIT_ERROR;
	}
	// chunk
	lua_getglobal( L, "string" );
	// chunk, string
	lua_getfield( L, -1, "dump" );
	// chunk, string, string.dump
	lua_pushvalue( L, -3 );
	// chunk, string, string.dump, chunk
	lua_pushboolean( L, nodebug );
	// chunk, string, string.dump, chunk, nodebug
	if( lua_pcall( L, 2, 1, 0 ) != 0 )
	{
		fprintf( stderr, "Failed to compile: %s\n", lua_tostring( L, -1 ) );
		return EXIT_ERROR;
	}
	// chunk, string, bytecode
	int length;
	char* data = lua_tolstring( L, -1, &length );
	FILE* output = fopen( argv[ 1 ], "wb" );
	if( !output )
	{
		perror( "Failed to open target file" );
		return EXIT_ERROR;
	}
	if( fwrite( data, 1, length, output ) != length )
	{
		perror( "Failed to write target file" );
		return EXIT_ERROR;
	}
	fclose( output );
	return EXIT_SUCCESS;
}
