#include <lua/lua.hpp>
#include <cstdio>
#include <cstring>

int l_exit( lua_State* L ) {
	bool* terminate = ( bool* )lua_touserdata( L, lua_upvalueindex( 1 ) );
	*terminate = true;
	return 0;
}

void printerror( lua_State* L ) {
	lua_getglobal( L, "tostring" );
	lua_insert( L, -2 );
	lua_pcall( L, 1, 1, 0 );
	printf( "error: %s\n", lua_tostring( L, -1 ) );
}

void executefile( lua_State* L, char const* path ) {
	if( luaL_loadfile( L, path ) != 0 ) {
		printerror( L );
	} else {
		if( lua_pcall( L, 0, 0, 0 ) != 0 ) {
			printerror( L );
		}
	}
	lua_settop( L, 0 );
}

void executestr( lua_State* L, char const* str ) {
	if( luaL_loadstring( L, str ) != 0 ) {
		printerror( L );
	} else {
		if( lua_pcall( L, 0, 0, 0 ) != 0 ) {
			printerror( L );
		}
	}
	lua_settop( L, 0 );
}

void executesval( lua_State* L, int index ) {
	executestr( L, lua_tostring( L, index ) );
}

char const* brackets_op[] = {
	"(", "{", "[",
	"function", "do", "repeat", "then" };
char const* brackets_cl[] = {
	")", "}", "]",
	"end", "until" };

char const* findsstr( char const* str, char const** target, int tcount ) {
	for( int i = 0; i < tcount; ++i ) {
		char const* strp = str;
		char const* tp = target[ i ];
		while( *tp != 0 ) {
			if( *tp != *strp ) {
				break;
			}
			++tp;
			++strp;
		}
		if( *tp == 0 ) {
			return strp;
		}
	}
	return 0;
}

int bracket_diff( char const* str ) {
	int result = 0;
	while( *str != 0 ) {
		char const* strp = findsstr( str, brackets_op, sizeof( brackets_op ) / sizeof( *brackets_op ) );
		if( strp ) {
			result += 1;
			str = strp;
			continue;
		}
		strp = findsstr( str, brackets_cl, sizeof( brackets_cl ) / sizeof( *brackets_cl ) );
		if( strp ) {
			result -= 1;
			str = strp;
			continue;
		}
		++str;
	}
	return result;
}

void interactive_input( lua_State* L ) {
	int top = lua_gettop( L );
	char input[ 1024 ];
	printf( "> " );
	gets( input );
	int bracketcount = bracket_diff( input );
	while( bracketcount != 0 ) {
		int ilen = strlen( input );
		if( ilen == 0 ) {
			break;
		}
		input[ ilen ] = '\n';
		input[ ilen + 1] = 0;
		lua_pushstring( L, input );
		printf( ">> " );
		gets( input );
		bracketcount += bracket_diff( input );
	}
	lua_pushstring( L, input );
	lua_concat( L, lua_gettop( L ) - top );
}

void interactive_run( lua_State* L ) {
	int top = lua_gettop( L );
	int success = false;
	lua_pushliteral( L, "print(" );
	lua_pushvalue( L, top );
	lua_pushliteral( L, ")" );
	lua_concat( L, 3 );
	size_t len;
	char const* str = lua_tolstring( L, top + 1, &len );
	if( luaL_loadbuffer( L, str, len, "interactive input" ) != 0 ) {
//		printerror( L );
		str = lua_tolstring( L, top, &len );
		if( luaL_loadbuffer( L, str, len, "interactive input" ) != 0 ) {
			printerror( L );
		} else {
			success = true;
		}
	} else {
		success = true;
	}
	if( success ) {
		if( lua_pcall( L, 0, 0, 0 ) != 0 ) {
			printerror( L );
		}
	}
	lua_settop( L, top - 1 );
}

void interactive( lua_State* L ) {
	printf( "Lua interactive input. Call \"exit()\" to quit.\n" );
	bool terminate = false;
	lua_pushlightuserdata( L, &terminate );
	lua_pushcclosure( L, &l_exit, 1 );
	lua_setglobal( L, "exit" );
	while( !terminate ) {
		lua_settop( L, 0 );
		interactive_input( L );
		interactive_run( L );
	}
}

void printversion() {
	printf( LUA_VERSION " interpreter\t" LUAJIT_VERSION "\n" LUAJIT_COPYRIGHT "\t" LUAJIT_URL "\n" );
}

void printhelp() {
	printf( "\n" );
	printf( "Syntax:\n" );
	printf( "lua [options...] [filename [parameters]]\n" );
	printf( "\n" );
	printf( "Available options:\n" );
	printf( "    -e chunk => Tries to load and run a string, prints a error on failure\n" );
	printf( "    -f filename => Tries to load and run a file, prints a error on failure\n" );
	printf( "    -g gname value => Sets the global gname to value as a string\n" );
	printf( "    -i => After executing all options and the main file (if present), enter\n" );
	printf( "the interactive mode\n" );
	printf( "    -l module => Requires module\n" );
	printf( "    -v => After exeuting all options, print version information\n" );
	printf( "    -? => After executing all options, print version information and this help\n" );
	printf( "\n" );
	printf( "    After executing all options, all command-line arguments are stored in the\n" );
	printf( "global arg as an array. Filename is stored at index 0, options following it are\n" );
	printf( "stored at positive indices, preceding ones are at negatives. E. g. if called:\n" );
	printf( "    > lua -g foo \"foo bar\" \"script.lua\" \"eggs\" 42\n" );
	printf( "arg would hold this table:\n" );
	printf( "{ [-4] = \"lua\",\n" );
	printf( "  [-3] = \"-g\",\n" );
	printf( "  [-2] = \"foo\",\n" );
	printf( "  [-1] = \"foo bar\",\n" );
	printf( "  [0] = \"script.lua\",\n" );
	printf( "  [1] = \"eggs\",\n" );
	printf( "  [2] = \"42\"}\n" );
	printf( "    Additionally, arguments that follow after the filename are passed to the\n" );
	printf( "script as function parameters, which are accessible with \"...\" construction.\n" );
	printf( "\n" );
	printf( "    In interactive mode, lines are executed immediately after they are\n" );
	printf( "finished, except when the last character is \\, in which case it is\n" );
	printf( "ignored and the interactive input is continued at the next line.\n" );
	printf( "    If the input is an expression or a comma-delimited sequence of expressions,\n" );
	printf( "its result is printed after its execution. Otherwise, nothing is written. You\n" );
	printf( "can suppress this automatic output by appending a semicolon (;) at the end,\n" );
	printf( "which will turn the input an invalid expression but valid chunk.\n" );
	printf( "    Every string, executed during the interactive mode, is loaded as a separate\n" );
	printf( "chunk. Because of that, locals are defined only for a single string, not for\n" );
	printf( "the entire mode.\n" );
//         "                                                                                "
}

int main( int argc, char const** argv ) {
	static char const* defarg[] = { 0, "-v", "-i" };
	int argpos = 1;
	lua_State* L = luaL_newstate();
	luaL_openlibs( L );
	bool enterinteractive = false;
	bool doprintversion = false;
	bool doprinthelp = false;
	if( argc == 1 ) {
		argc = 3;
		defarg[ 0 ] = argv[ 0 ];
		argv = defarg;
	}
	while( argpos < argc ) {
		char const* arg = argv[ argpos ];
		if( strcmp( arg, "-e" ) == 0 ) {
			if( argpos + 1 < argc ) {
				argpos += 1;
				executestr( L, argv[ argpos ] );
			}
		} else if( strcmp( arg, "-l" ) == 0 ) {
			if( argpos + 1 < argc ) {
				argpos += 1;
				lua_pushliteral( L, "require(\"" );
				lua_pushstring( L, argv[ argpos ] );
				lua_pushliteral( L, "\")" );
				lua_concat( L, 3 );
				executesval( L, -1 );
			}
		} else if( strcmp( arg, "-i" ) == 0 ) {
			enterinteractive = true;
		} else if( strcmp( arg, "-?" ) == 0 ) {
			doprintversion = true;
			doprinthelp = true;
		} else if( strcmp( arg, "-v" ) == 0 ) {
			doprintversion = true;
		} else if( strcmp( arg, "-g" ) == 0 ) {
			if( argpos + 1 < argc ) {
				argpos += 1;
				if( argpos + 1 < argc ) {
					argpos += 1;
					lua_pushstring( L, argv[ argpos ] );
					lua_setglobal( L, argv[ argpos - 1 ] );
				}
			}
		} else if( strcmp( arg, "-f" ) == 0 ) {
			if( argpos + 1 < argc ) {
				argpos += 1;
				executefile( L, argv[ argpos ] );
			}
		} else {
			break;
		}
		argpos += 1;
	}
	if( doprintversion ) {
		printversion();
	}
	if( doprinthelp ) {
		printhelp();
	}
	if( argpos < argc ) {
		if( luaL_loadfile( L, argv[ argpos ] ) != 0 ) {
			printerror( L );
		} else {
			lua_createtable( L, argc, 0 );
			for( int i = 0; i < argc; ++i ) {
				lua_pushstring( L, argv[ i ] );
				lua_rawseti( L, -2, i - argpos );
			}
			lua_setglobal( L, "arg" );
			argpos += 1;
			int sargc = argc - argpos;
			for( ; argpos < argc; ++argpos ) {
				lua_pushstring( L, argv[ argpos ] );
			}
			if( lua_pcall( L, sargc, 0, 0 ) != 0 ) {
				printerror( L );
			}
		}
		lua_settop( L, 0 );
	}
	if( enterinteractive ) {
		interactive( L );
	}
	lua_close( L );
	return 0;
}
