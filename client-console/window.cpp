#include "window.hpp"
#include "hostmethodlist.hpp"
#include <utils/strexception.hpp>
#include <utils/profile.hpp>
#include <utils/encoding.hpp>
#include <utils/cbase.hpp>
#include <cstdlib>
#include <cstdio>

#undef PROFILE
#define PROFILE( name, op ) { op }

namespace window
{
#define marker() /* LOG( m_console, "-" ) */ ( void )( 0 )

#if defined( _WIN32 ) || defined( _WIN64 )
#define PATH ".\\"
#define PLATFORM "win"
#elif defined(__ANDROID__)
#define PATH "/storage/sdcard0/projects/output/"
#define PLATFORM "android"
#endif
#define MAINCONFIG_PATH PATH "main.lc"
#define MAINCONFIG_STR "_PLATFORM = '" PLATFORM "'; _CONSOLE = true; _PATH = [[" PATH "]]"

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
		, m_console( utils::Console )
	{
		marker();
		m_mainconfig.runcmd( m_cmdline );
		initialize();
	}

	Window::~Window()
	{
		marker();
		if( m_lstate )
		{
			lua_close( m_lstate );
		}
	}

	int Window::mainloop()
	{
		return 0;
	}

	void Window::initialize()
	{
		marker();
		m_info.width = 0;
		m_info.height = 0;
		m_info.texelsoffset = 0;
		m_info.texeltoffset = 0;
		m_info.acceleratorinput = false;
		m_info.pointinput = false;
		m_info.keyboardinput = false;
		m_info.cmdline = m_cmdline.getchars();
		m_info.silent = true;
		initlstate();
	}

	void Window::initlstate()
	{
		utils::String bootscript = m_mainconfig.string(
			"bootscript" );
		LOG( m_console, "~ Boot script location: %s", bootscript.getchars() );
		if( bootscript )
		{
			m_lstate = luaL_newstate();
			lua_settop( m_lstate, 0 );
			luaL_openlibs( m_lstate );
			if( luaL_loadfile( m_lstate, bootscript ) != 0 )
			{
				lua_throwerror( m_lstate );
			}
			lua_pushlightuserdata( m_lstate, this );
			lua_pushlightuserdata( m_lstate, gethostmethodlist() );
			lua_pushstring( m_lstate, PATH );
			lua_pushstring( m_lstate, MAINCONFIG_STR );
			if( lua_pcall( m_lstate, 4, 0, 0 ) != 0 )
			{
				lua_throwerror( m_lstate );
			}
		}
		else
		{
			CriticalError_m( "bootscript undefined" );
		}
	}

	void Window::setshape( graphics::Shape* nv )
	{
		throw std::runtime_error( "application is in console mode" );
	}

	WindowInfo const* Window::windowinfo()
	{
		return &m_info;
	}

	void Window::finish()
	{
	}

	bool window_window_setshape(
		Window* window, graphics::Shape* shape ) noexcept
	{
	CBASE_PROTECT(
		window->setshape( shape );
		return 1;
	)
	}

	WindowInfo const* window_window_getinfo( Window* window ) noexcept
	{
	CBASE_PROTECT(
		return window->windowinfo();
	)
	}

	bool window_window_finish( Window* window ) noexcept
	{
	CBASE_PROTECT(
		window->finish();
		return 1;
	)
	}
}
