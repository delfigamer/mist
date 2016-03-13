#include <client-main/window.hpp>
#include <client-main/methodlist.hpp>
#include <client-main/luainit.hpp>
#include <utils/strexception.hpp>
#include <utils/profile.hpp>
#include <utils/encoding.hpp>
#include <utils/cbase.hpp>
#include <osapi.hpp>
#include <cstdlib>
#include <cstdio>

#undef PROFILE
#define PROFILE( name, op ) { op }

namespace window
{
#define marker() /* LOG( "-" ), */ ( void )( 0 )

#if defined( _WIN32 ) || defined( _WIN64 )
#define PLATFORM "win"
#elif defined(__ANDROID__)
#define PLATFORM "android"
#endif
#define MAINCONFIG_PATH PATH_START "main.lc"
#define MAINCONFIG_STR "_PLATFORM = '" PLATFORM "'; _PATH = [[" PATH_START "]]"

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

#if defined( _WIN32 ) || defined( _WIN64 )
	wchar_t const* Window::ClassName = L"mainwindow";
	wchar_t const* Window::WindowCaption = L"Main window";

	int const DefaultWindowSize[] = { 640, 480 };

	static bool Errored = false;

	void CheckWinError(
		char const* file, char const* function, int line )
	{
		DWORD LastError = GetLastError();
		if( !LastError )
		{
			return;
		}
		Errored = true;
		char StrBuffer[ 1024 ];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
			0,
			LastError,
			MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
			StrBuffer,
			sizeof( StrBuffer ) - 1,
			0 );
		CriticalError( file, function, line, StrBuffer );
	}
#define CheckWinError() CheckWinError( \
	__FILE__, __FUNCTION__, __LINE__ )

	LRESULT CALLBACK GlobalWindowProc(
		HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		Window* window = ( Window* )GetClassLongPtr( hwnd, 0 );
		if( !Errored && ( !window || ( window->m_hwnd != hwnd ) ) )
		{
			return DefWindowProcW( hwnd, uMsg, wParam, lParam );
		}
		else
		{
			return window->WindowProc( uMsg, wParam, lParam );
		}
	}

	LRESULT Window::WindowProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
		case WM_DESTROY:
			pushevent( Event::Close );
			m_finishrequired = true;
			// PostQuitMessage( 0 );
			return 0;
		case WM_PAINT:
			paint();
			return 0;
		case WM_KEYDOWN:
			pushevent( Event::KeyDown, wParam );
			return 0;
		case WM_KEYUP:
			pushevent( Event::KeyUp, wParam );
			return 0;
		case WM_CHAR:
			pushevent( Event::Char, wParam );
			return 0;
		case WM_MOUSEMOVE:
			pointmove( 0, true, lParam );
			pointmove( 1, ( wParam & MK_LBUTTON ) != 0, lParam );
			pointmove( 2, ( wParam & MK_RBUTTON ) != 0, lParam );
			pointmove( 3, ( wParam & MK_MBUTTON ) != 0, lParam );
			return 0;
		case WM_LBUTTONDOWN:
			pointmove( 1, true, lParam );
			return 0;
		case WM_RBUTTONDOWN:
			pointmove( 2, true, lParam );
			return 0;
		case WM_MBUTTONDOWN:
			pointmove( 3, true, lParam );
			return 0;
		case WM_LBUTTONUP:
			pointmove( 1, false, lParam );
			return 0;
		case WM_RBUTTONUP:
			pointmove( 2, false, lParam );
			return 0;
		case WM_MBUTTONUP:
			pointmove( 3, false, lParam );
			return 0;
		case WM_ACTIVATE:
			pushevent( Event::Focus,
				wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE );
			return 0;
		default:
			return DefWindowProcW( m_hwnd, uMsg, wParam, lParam );
		}
	}

	void Window::pointmove(
		int point, bool newstate, LPARAM lParam )
	{
		if( m_pointstate[ point ] != newstate )
		{
			m_pointstate[ point ] = newstate;
			if( newstate )
			{
				pushevent( Event::PointDown, point,
					GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
			}
			else
			{
				pushevent( Event::PointUp, point );
			}
		}
		else if( newstate )
		{
			pushevent( Event::PointMove, point,
				GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
		}
	}

	Window::Window( WindowCreationData const& wcd )
		: m_hwnd( 0 )
		, m_pointstate{ 0 }
		, m_terminated( false )
		, m_mainconfig( MAINCONFIG_PATH, MAINCONFIG_STR )
		, m_finishrequired( false )
		, m_finished( false )
	{
		marker();
		m_mainconfig.runcmd( wcd.cmdline );
		m_silent = m_mainconfig.boolean( "silent", false );
		m_fpscounter = 0;
		m_tpscounter = 0;
		m_fpstime = 0x7fffffff;
		m_tpstime = 0x7fffffff;
		if( !m_silent )
		{
			m_wndclass.style = CS_VREDRAW | CS_HREDRAW;
			m_wndclass.lpfnWndProc = &GlobalWindowProc;
			m_wndclass.cbClsExtra = 8;
			m_wndclass.cbWndExtra = 0;
			m_wndclass.hInstance = wcd.hInstance;
			m_wndclass.hIcon = LoadIcon( 0, IDI_APPLICATION );
			m_wndclass.hCursor = LoadCursor( 0, IDC_ARROW );
			m_wndclass.hbrBackground = 0;
			m_wndclass.lpszMenuName = 0;
			m_wndclass.lpszClassName = ClassName;
			if( !RegisterClassW( &m_wndclass ) )
			{
				CheckWinError();
			}
			HWND dummy = CreateWindowW(
				ClassName,
				L"dummy",
				WS_DISABLED,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				m_wndclass.hInstance,
				0 );
			if( !dummy )
			{
				CheckWinError();
			}
			if( !SetClassLongPtr( dummy, 0, LONG_PTR( this ) ) )
			{
				CheckWinError();
			}
			if( !DestroyWindow( dummy ) )
			{
				CheckWinError();
			}
			RECT WindowRect = {
				0,
				0,
				m_mainconfig.integer(
					"windowsize[1] or windowsize.x",
					DefaultWindowSize[ 0 ] ),
				m_mainconfig.integer(
					"windowsize[2] or windowsize.y",
					DefaultWindowSize[ 1 ] ) };
			if( !AdjustWindowRect(
				&WindowRect, WindowStyle, false ) )
			{
				CheckWinError();
			}
			m_hwnd = CreateWindowW(
				ClassName,
				WindowCaption,
				WindowStyle,
				20,
				20,
				WindowRect.right - WindowRect.left,
				WindowRect.bottom - WindowRect.top,
				0,
				0,
				m_wndclass.hInstance,
				0 );
			if( !m_hwnd )
			{
				CheckWinError();
			}
			ShowWindow( m_hwnd, SW_SHOW );
		}
		initialize();
		if( !m_silent )
		{
			UpdateWindow( m_hwnd );
		}
	}

	Window::~Window()
	{
	}

	int Window::mainloop()
	{
		MSG message = { 0, 0, 0, 0 };
		while( !m_terminated )
		{
			if( !m_silent )
			{
				if( PeekMessageW(
					&message, m_hwnd, 0, 0, PM_REMOVE ) )
				{
					TranslateMessage( &message );
					DispatchMessageW( &message );
				}
			}
			if( m_silent  || m_finishrequired
				|| m_finished.load( std::memory_order_relaxed ) )
			{
				finalize();
				m_terminated = true;
			}
		}
		return 0;
	}
#elif defined(__ANDROID__)
	int32_t g_handle_input( android_app* app, AInputEvent* event )
	{
		Window* window = ( Window* )app->userData;
		return window->handle_input( event );
	}

	void g_handle_cmd( android_app* app, int32_t cmd )
	{
		Window* window = ( Window* )app->userData;
		window->handle_cmd( cmd );
	}

	int32_t Window::handle_input( AInputEvent* event )
	{
		marker();
		switch( AInputEvent_getType( event ) )
		{
		case AINPUT_EVENT_TYPE_MOTION:
		{
			int action = AMotionEvent_getAction( event );
			int index = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			action &= AMOTION_EVENT_ACTION_MASK;
			switch( action )
			{
			case AMOTION_EVENT_ACTION_DOWN:
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
				pointevent(
					"pointdown",
					index + 1,
					AMotionEvent_getX( event, 0 ),
					AMotionEvent_getY( event, 0 ) );
				break;
			case AMOTION_EVENT_ACTION_UP:
			case AMOTION_EVENT_ACTION_CANCEL:
				pointevent(
					"pointup",
					AMotionEvent_getPointerId( event, 0 ) + 1,
					-1,
					-1 );
				break;
			case AMOTION_EVENT_ACTION_POINTER_UP:
				pointevent(
					"pointup",
					index + 1,
					-1,
					-1 );
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				for(
					int i = 0;
					i < int( AMotionEvent_getPointerCount( event ) );
					++i )
				{
				pointevent(
					"pointmove",
					AMotionEvent_getPointerId( event, i ) + 1,
					AMotionEvent_getX( event, i ),
					AMotionEvent_getY( event, i ) );
				}
				break;
			}
			return 0;
		} break;
		}
		return 0;
	}

	void Window::handle_cmd( int32_t cmd )
	{
		switch( cmd )
		{
		case APP_CMD_SAVE_STATE:
			marker();
			m_app->savedStateSize = 0;
			break;
		case APP_CMD_INIT_WINDOW:
			marker();
			if( m_app->window != 0 )
			{
				initialize();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			marker();
			m_finished = true;
			break;
		case APP_CMD_GAINED_FOCUS:
			marker();
			if( m_accelerometerSensor != 0 )
			{
				ASensorEventQueue_enableSensor(
					m_sensorEventQueue, m_accelerometerSensor );
				ASensorEventQueue_setEventRate(
					m_sensorEventQueue, m_accelerometerSensor,
					1000000/60 );
			}
			// notifyevent( "focusgained" );
			break;
		case APP_CMD_LOST_FOCUS:
			marker();
			if( m_accelerometerSensor != 0 )
			{
				ASensorEventQueue_disableSensor(
					m_sensorEventQueue, m_accelerometerSensor );
			}
			// notifyevent( "focuslost" );
			break;
		}
		marker();
	}

	void Window::handle_sensor( ASensorEvent* event )
	{
		// lmessagebegin();
			// m_lstate.pushstring( "acceleration" );
			// m_lstate.pushnumber( event->acceleration.x );
			// m_lstate.pushnumber( event->acceleration.y );
			// m_lstate.pushnumber( event->acceleration.z );
		// lmessageend();
	}

	Window::Window( WindowCreationData const& wcd )
		: m_app( wcd.app )
		, m_mainconfig( MAINCONFIG_PATH, MAINCONFIG_STR )
		, m_lstateready( false )
		, m_finishrequired( false )
		, m_finished( false )
	{
		marker();
		m_mainconfig.runcmd( wcd.cmdline );
		m_silent = m_mainconfig.boolean( "silent", false );
		if( !m_silent )
		{
			m_app->userData = this;
			m_app->onAppCmd = &g_handle_cmd;
			m_app->onInputEvent = &g_handle_input;
			m_sensorManager = ASensorManager_getInstance();
			m_accelerometerSensor = ASensorManager_getDefaultSensor(
				m_sensorManager, ASENSOR_TYPE_ACCELEROMETER );
			m_sensorEventQueue = ASensorManager_createEventQueue(
				m_sensorManager, m_app->looper, LOOPER_ID_USER, 0, 0 );
			if( m_app->savedState )
			{
			}
		}
	}

	Window::~Window()
	{
		marker();
	}

	int Window::mainloop()
	{
		marker();
		if( m_silent )
		{
			finalize();
			return 0;
		}
		while( true )
		{
			int ident;
			int events;
			android_poll_source* source;
			while( ( ident = ALooper_pollAll(
				0, 0, &events, ( void** )&source ) ) >= 0 )
			{
				if( source )
				{
					source->process( m_app, source );
				}
				if( ident == LOOPER_ID_USER )
				{
					if( m_accelerometerSensor )
					{
						ASensorEvent event;
						while( ASensorEventQueue_getEvents(
							m_sensorEventQueue, &event, 1 ) > 0 )
						{
							handle_sensor( &event );
						}
					}
				}
				if( m_finished.load( std::memory_order_relaxed ) ||
					m_app->destroyRequested )
				{
					finalize();
					return 0;
				}
			}
			paint();
		}
	}

#endif

	void Window::initialize()
	{
		marker();
		if( !m_silent )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			m_display.initialize( m_mainconfig, m_hwnd );
#elif defined( __ANDROID__ )
			m_display.initialize( m_mainconfig, m_app );
#endif
			auto dinfo = m_display.getdisplayinfo();
			m_info.width = dinfo->width;
			m_info.height = dinfo->height;
			m_info.texelsoffset = dinfo->texelsoffset;
			m_info.texeltoffset = dinfo->texeltoffset;
#if defined( _WIN32 ) || defined( _WIN64 )
			m_info.acceleratorinput = false;
			m_info.pointinput = true;
			m_info.keyboardinput = true;
#elif defined( __ANDROID__ )
			m_info.acceleratorinput = false;
			m_info.pointinput = true;
			m_info.keyboardinput = false;
#endif
		}
		else
		{
			m_info.width = 0;
			m_info.height = 0;
			m_info.texelsoffset = 0;
			m_info.texeltoffset = 0;
			m_info.acceleratorinput = false;
			m_info.pointinput = false;
			m_info.keyboardinput = false;
		}
		m_info.configset = &m_mainconfig;
		m_info.silent = m_silent;
		m_info.methodlist = &methodlist;
		m_info.window = this;
		m_fpstime = clock() + CLOCKS_PER_SEC;
		initlstate();
	}

	void Window::finalize()
	{
		closelstate();
		if( !m_silent )
		{
			m_display.finalize();
		}
	}

	void Window::paint()
	{
	PROFILE( "paint",
		if( !m_silent )
		{
			m_display.paint();
			m_fpscounter += 1;
			clock_t time = clock();
			if( time > m_fpstime )
			{
				m_fps = m_fpscounter;
				m_fpscounter = 0;
				m_fpstime = time + CLOCKS_PER_SEC;
			}
#if defined( _WIN32 ) || defined( _WIN64 )
			char buffer[ 256 ];
			snprintf(
				buffer, sizeof( buffer ),
				"FPS=%.3i; TPS=%.3i", m_fps, m_tps );
			SetWindowText( m_hwnd, buffer );
#endif
		}
	)
	}

	utils::String lua_retrieveerror( lua_State* L )
	{
		lua_getglobal( L, "tostring" );
		lua_insert( L, -2 );
		lua_pcall( L, 1, 1, 0 );
		utils::String message( lua_tostring( L, -1 ) );
		lua_pop( L, 1 );
		return message;
	}

	void Window::luathreadfunc()
	{
		LOG( "~ Lua thread started" );
		try
		{
			lua_State* L = luaL_newstate();
			luaL_openlibs( L );
			if( luaL_loadbuffer(
				L,
				luainit[ 0 ].data, luainit[ 0 ].length, luainit[ 0 ].name ) != 0 )
			{
				utils::String error = lua_retrieveerror( L );
				LOG( "%s", error.getchars() );
				goto end;
			}
			lua_pushlightuserdata( L, &luainit[ 1 ] );
			lua_pushlightuserdata( L, &m_info );
			if( lua_pcall( L, 2, 1, 0 ) != 0 )
			{
				utils::String error = lua_retrieveerror( L );
				LOG( "%s", error.getchars() );
			}
		end:
			lua_close( L );
		}
		catch( std::exception const& e )
		{
			LOG( "! %s", e.what() );
		}
		catch( ... )
		{
			LOG( "!" );
		}
		m_finished.store( true, std::memory_order_relaxed );
		LOG( "~ Lua thread finished" );
	}

	void Window::initlstate()
	{
		m_luathread = std::thread( &Window::luathreadfunc, this );
		m_timeoffset = clock();
		m_tpstime = clock() + CLOCKS_PER_SEC;
	}

	void Window::closelstate()
	{
		clock_t time = clock() + CLOCKS_PER_SEC * 5;
		while( !m_finished.load( std::memory_order_relaxed ) )
		{
			if( clock() > time )
			{
				LOG( "Lua thread takes too long to finish, "
					"terminating immediately" );
				std::exit( 0 );
			}
		}
		m_luathread.join();
	}

	void Window::pushevent( int name, int a1, int a2, int a3, int a4 )
	{
		m_eventqueue.push( Event{ name, a1, a2, a3, a4 } );
	}

	// PROFILE( "pointevent",
		// PointEventData* edata = ( PointEventData* )vedata;
		// lua_pushvalue( L, 1 );
		// lua_pushstring( L, edata->event );
		// lua_pushinteger( L, edata->point );
		// int callresult;
		// if( edata->x >= 0 && edata->y >= 0 )
		// {
			// lua_pushinteger( L, edata->x );
			// lua_pushinteger( L, edata->y );
			// callresult = lua_pcall( L, 4, 0, 0 );
		// }
		// else
		// {
			// callresult = lua_pcall( L, 2, 0, 0 );
		// }
		// if( callresult != 0 )
		// {
			// *error = lua_retrieveerror( L );
		// }
		// delete edata;
	// )

	// PROFILE( "keyevent",
		// KeyEventData* edata = ( KeyEventData* )vedata;
		// lua_pushvalue( L, 1 );
		// lua_pushstring( L, edata->event );
		// lua_pushinteger( L, edata->key );
		// if( lua_pcall( L, 2, 0, 0 ) != 0 )
		// {
			// *error = lua_retrieveerror( L );
		// }
		// delete edata;
	// )

	// PROFILE( "charevent",
		// CharEventData* edata = ( CharEventData* )vedata;
		// char buffer[ 5 ];
		// size_t pointsize;
		// if( utils::encoding::utf8.encode(
			// buffer, edata->ch, sizeof( buffer ) - 1, &pointsize ) )
		// {
			// buffer[ pointsize ] = 0;
			// lua_pushvalue( L, 1 );
			// lua_pushstring( L, edata->event );
			// lua_pushstring( L, buffer );
			// if( lua_pcall( L, 2, 0, 0 ) != 0 )
			// {
				// *error = lua_retrieveerror( L );
			// }
		// }
		// delete edata;
	// )

	// PROFILE( "notifyevent",
		// lua_pushvalue( L, 1 );
		// lua_pushstring( L, ( char const* )vedata );
		// if( lua_pcall( L, 1, 0, 0 ) != 0 ) {
			// *error = lua_retrieveerror( L );
		// }
	// )

		// marker();
		// lua_pushvalue( L, 1 );
		// lua_pushstring( L, "destroy" );
		// if( lua_pcall( L, 1, 0, 0 ) != 0 )
		// {
			// *error = lua_retrieveerror( L );
		// }
		// marker();

		// Window* self = ( Window* )vedata;
		// if( self->m_lstateready )
		// {
			// marker();
			// clock_t time = clock();
			// if( lua_isnoneornil( L, 1 ) )
			// {
				// return;
			// }
			// lua_pushvalue( L, 1 );
			// lua_pushstring( L, "tick" );
			// lua_pushnumber(
				// L,
				// float( time - self->m_timeoffset ) / CLOCKS_PER_SEC );
			// if( lua_pcall( L, 2, 0, 0 ) != 0 )
			// {
				// *error = lua_retrieveerror( L );
			// }
			// self->m_timeoffset = time;
			// self->m_tpscounter += 1;
			// time = clock();
			// if( time > self->m_tpstime )
			// {
				// self->m_tps = self->m_tpscounter;
				// self->m_tpscounter = 0;
				// self->m_tpstime = time + CLOCKS_PER_SEC;
			// }
			// marker();
		// }

	void Window::setshape( graphics::Shape* nv )
	{
		m_display.setshape( nv );
	}

	bool Window::popevent( Event* event )
	{
		return m_eventqueue.pop( event );
	}
}
