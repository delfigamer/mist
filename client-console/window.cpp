#include <client-console/window.hpp>
#include <client-console/luainit.hpp>
#include <client-console/methodlist.hpp>
#include <utils/strexception.hpp>
#include <utils/encoding.hpp>
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <osapi.hpp>
#include <cstdlib>
#include <cstdio>

#if defined( _MSC_VER )
#include <cstdarg>
static int snprintf( char* buffer, int buflen, char const* format, ... )
{
	va_list arg;
	va_start( arg, format );
	int result = vsnprintf_s( buffer, buflen, buflen, format, arg );
	va_end( arg );
	return result;
}
#endif

namespace window
{
#if defined( _WIN32 ) || defined( _WIN64 )
#define PLATFORM "win"
#elif defined(__ANDROID__)
#define PLATFORM "android"
#endif
#define MAINCONFIG_PATH PATH_START "main.lc"
#define MAINCONFIG_STR "_PLATFORM = '" PLATFORM "'; _CONSOLE = true; _PATH = [[" PATH_START "]]"

	void CriticalError(
		char const* file, char const* function, int line,
		char const* msg )
	{
		char StrBuffer[ 1024 ];
		snprintf(
			StrBuffer, sizeof( StrBuffer ),
			"[%s : %s @ %i] %s", file, function, line, msg );
		throw utils::StrException( StrBuffer );
	}

#define CriticalError_m( msg ) CriticalError( \
	__FILE__, __FUNCTION__, __LINE__, msg )

	utils::String lua_throwerror( lua_State* L )
	{
		lua_getglobal( L, "tostring" );
		lua_insert( L, -2 );
		lua_pcall( L, 1, 1, 0 );
		utils::String message( lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
		throw utils::StrException( message );
	}

	Window::Window( WindowCreationData const& wcd )
		: m_mainconfig( MAINCONFIG_PATH, MAINCONFIG_STR )
		, m_cmdline( wcd.cmdline )
		, m_lstate( 0 )
	{
		m_mainconfig.runcmd( m_cmdline );
		initialize();
	}

	Window::~Window()
	{
		if( m_lstate )
		{
			lua_close( m_lstate );
		}
	}

	void Window::mainloop()
	{
	}

	void Window::initialize()
	{
		m_info.window = this;
		m_info.configset = &m_mainconfig;
		m_info.acceleratorinput = false;
		m_info.pointinput = false;
		m_info.keyboardinput = false;
		m_info.silent = true;
		m_info.client_methodlist = &client_console_methodlist;
		m_info.renderer_methodlist = nullptr;
		m_info.renderer_module = nullptr;
		m_info.width = 0;
		m_info.height = 0;
		m_info.texelsoffset = 0;
		m_info.texeltoffset = 0;
		initlstate();
	}

	void Window::initlstate()
	{
		m_lstate = luaL_newstate();
		luaL_openlibs( m_lstate );
		if( luaL_loadbuffer(
			m_lstate,
			luainit[ 0 ].data, luainit[ 0 ].length, luainit[ 0 ].name ) != 0 )
		{
			lua_throwerror( m_lstate );
		}
		lua_pushlightuserdata( m_lstate, &luainit[ 1 ] );
		lua_pushlightuserdata( m_lstate, &m_info );
		if( lua_pcall( m_lstate, 2, 0, 0 ) != 0 )
		{
			lua_throwerror( m_lstate );
		}
	}
}
