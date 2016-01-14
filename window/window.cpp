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
#define MAINCONFIG_STR "_PLATFORM = '" PLATFORM "'; _PATH = [[" PATH "]]"

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
		if( !Errored && ( !window || (window->m_hwnd != hwnd ) ) )
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
		marker();
		switch( uMsg ) {
			case WM_DESTROY:
				finalize();
				m_terminated = true;
				// PostQuitMessage( 0 );
				return 0;
			case WM_PAINT:
				paint();
				return 0;
			case WM_KEYDOWN:
				keyevent( "keydown", wParam );
				return 0;
			case WM_KEYUP:
				keyevent( "keyup", wParam );
				return 0;
			case WM_CHAR:
				charevent( "char", wParam );
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
				if( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE )
				{
					notifyevent( "focusgained" );
				}
				else
				{
					notifyevent( "focuslost" );
				}
				return 0;
			default:
				return DefWindowProcW( m_hwnd, uMsg, wParam, lParam );
		}
	}

	void Window::pointmove(
		int point, bool newstate, LPARAM lParam )
	{
		marker();
		if( m_pointstate[ point ] != newstate )
		{
			m_pointstate[ point ] = newstate;
			if( newstate )
			{
				pointevent( "pointdown", point,
					GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
			}
			else
			{
				pointevent( "pointup", point, -1, -1 );
			}
		}
		else if( newstate )
		{
			pointevent( "pointmove", point,
				GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
		}
	}

	Window::Window( WindowCreationData const& wcd )
		: m_hwnd( 0 )
		, m_pointstate{ 0 }
		, m_terminated( false )
		, m_mainconfig( MAINCONFIG_PATH, MAINCONFIG_STR )
		, m_cmdline( wcd.cmdline )
		, m_lstateready( false )
		, m_finished( false )
		, m_console( utils::Console )
	{
		marker();
		m_mainconfig.runcmd( m_cmdline );
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
		marker();
	}

	int Window::mainloop()
	{
		marker();
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
			if( m_silent || m_finished )
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
			int index = action >>
				AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			action &= AMOTION_EVENT_ACTION_MASK;
			switch( action )
			{
			case AMOTION_EVENT_ACTION_DOWN:
				// pointevent(
					// "pointdown", index + 1,
					// AMotionEvent_getX( event, 0 ),
					// AMotionEvent_getY( event, 0 ) );
				break;
			case AMOTION_EVENT_ACTION_UP:
			case AMOTION_EVENT_ACTION_CANCEL:
				// pointevent(
					// "pointup", index + 1, -1, -1 );
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				for(
					int i = 0;
					i < int( AMotionEvent_getPointerCount( event ) );
					++i )
				{
				// pointevent(
					// "pointmove",
					// AMotionEvent_getPointerId( event, i ) + 1,
					// AMotionEvent_getX( event, i ),
					// AMotionEvent_getY( event, i ) );
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
		, m_cmdline( wcd.cmdline )
		, m_lstateready( false )
		, m_finished( false )
		, m_console( utils::Console )
	{
		marker();
		m_mainconfig.runcmd( m_cmdline );
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
				if( m_finished || m_app->destroyRequested )
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
			// m_display.initialize( m_mainconfig, m_app );
#endif
			auto dinfo = m_display.displayinfo();
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
		m_info.cmdline = m_cmdline.getchars();
		m_info.silent = m_silent;
		m_fpstime = clock() + CLOCKS_PER_SEC;
		initlstate();
	}

	void Window::finalize()
	{
		marker();
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

	void Window::pointevent(
		char const* event, int point, int x, int y )
	{
		if( !m_lstateready )
		{
			return;
		}
		struct Msg
		{
			char const* event;
			int point;
			int x;
			int y;

			static void action( void* vself, lua_State* L, utils::String* error )
			{
			PROFILE( "pointevent",
				Msg* self = ( Msg* )vself;
				lua_pushvalue( L, 1 );
				lua_pushstring( L, self->event );
				lua_pushinteger( L, self->point );
				int callresult;
				if( self->x >= 0 && self->y >= 0 )
				{
					lua_pushinteger( L, self->x );
					lua_pushinteger( L, self->y );
					callresult = lua_pcall( L, 4, 0, 0 );
				}
				else
				{
					callresult = lua_pcall( L, 2, 0, 0 );
				}
				if( callresult != 0 )
				{
					*error = lua_retrieveerror( L );
				}
				delete self;
			)
			}
		};
		Msg* msg = new Msg{ event, point, x, y };
		m_lstate.schedule( &Msg::action, msg );
	}

	void Window::keyevent( char const* event, int key )
	{
		if( !m_lstateready )
		{
			return;
		}
		struct Msg
		{
			char const* event;
			int key;

			static void action( void* vself, lua_State* L, utils::String* error )
			{
			PROFILE( "keyevent",
				Msg* self = ( Msg* )vself;
				lua_pushvalue( L, 1 );
				lua_pushstring( L, self->event );
				lua_pushinteger( L, self->key );
				if( lua_pcall( L, 2, 0, 0 ) != 0 )
				{
					*error = lua_retrieveerror( L );
				}
				delete self;
			)
			}
		};
		Msg* msg = new Msg{ event, key };
		m_lstate.schedule( &Msg::action, msg );
	}

	void Window::charevent( char const* event, int ch )
	{
		if( !m_lstateready )
		{
			return;
		}
		struct Msg
		{
			char const* event;
			int ch;

			static void action( void* vself, lua_State* L, utils::String* error )
			{
			PROFILE( "charevent",
				Msg* self = ( Msg* )vself;
				char buffer[ 5 ];
				size_t pointsize;
				if( utils::encoding::utf8.encode(
					buffer, self->ch, sizeof( buffer ) - 1, &pointsize ) )
				{
					buffer[ pointsize ] = 0;
					lua_pushvalue( L, 1 );
					lua_pushstring( L, self->event );
					lua_pushstring( L, buffer );
					if( lua_pcall( L, 2, 0, 0 ) != 0 )
					{
						*error = lua_retrieveerror( L );
					}
				}
				delete self;
			)
			}
		};
		Msg* msg = new Msg{ event, ch };
		m_lstate.schedule( &Msg::action, msg );
	}

	void Window::notifyevent( char const* event )
	{
		if( !m_lstateready )
		{
			return;
		}
		struct Msg
		{
			static void action( void* vevent, lua_State* L, utils::String* error )
			{
			PROFILE( "notifyevent",
				lua_pushvalue( L, 1 );
				lua_pushstring( L, ( char const* )vevent );
				if( lua_pcall( L, 1, 0, 0 ) != 0 ) {
					*error = lua_retrieveerror( L );
				}
			)
			}
		};
		m_lstate.schedule( &Msg::action, ( void* )event );
	}

	void Window::initlstate()
	{
		utils::String bootscript = m_mainconfig.string(
			"bootscript" );
		LOG( m_console, "~ Boot script location: %s", bootscript.getchars() );
		if( bootscript )
		{
			m_lstate.applyconfig( m_mainconfig );
			struct Msg
			{
				Window* window;
				utils::String bootscript;
				static void action(
					void* vself, lua_State* L, utils::String* error )
				{
					Msg* self = ( Msg* )vself;
					lua_settop( L, 0 );
					luaL_openlibs( L );
					if( luaL_loadfile( L, self->bootscript ) != 0 )
					{
						*error = lua_retrieveerror( L );
						self->window->m_finished = true;
						delete self;
						return;
					}
					lua_pushlightuserdata( L, self->window );
					lua_pushlightuserdata( L, gethostmethodlist() );
					lua_pushstring( L, PATH );
					lua_pushstring( L, MAINCONFIG_STR );
					if( lua_pcall( L, 4, 1, 0 ) != 0 )
					{
						*error = lua_retrieveerror( L );
						self->window->m_finished = true;
					}
					delete self;
				}
			};
			Msg* msg = new Msg{ this, bootscript };
			m_lstate.schedule( &Msg::action, msg );
		}
		else
		{
			CriticalError_m( "bootscript undefined" );
		}
		m_lstateready = true;
		m_timeoffset = clock();
		m_tpstime = clock() + CLOCKS_PER_SEC;
		m_lstate.setdefaultaction(
			&AsyncLuaState::action_t::bind_method< Window, &Window::ticklstate >,
			this );
	}

	void Window::closelstate()
	{
		if( !m_lstateready )
		{
			return;
		}
		struct Msg
		{
			static void action( void* vself, lua_State* L, utils::String* error )
			{
				marker();
				lua_pushvalue( L, 1 );
				lua_pushstring( L, "destroy" );
				if( lua_pcall( L, 1, 0, 0 ) != 0 )
				{
					*error = lua_retrieveerror( L );
				}
				marker();
			}
		};
		marker();
		m_lstateready = false;
		m_lstate.setdefaultaction();
		m_lstate.schedule( &Msg::action );
		m_lstate.join();
		marker();
	}

	void Window::ticklstate( lua_State* L, utils::String* error )
	{
		if( m_lstateready )
		{
		// PROFILE( "tick",
			marker();
			clock_t time = clock();
			if( lua_isnoneornil( L, 1 ) )
			{
				return;
			}
			lua_pushvalue( L, 1 );
			lua_pushstring( L, "tick" );
			lua_pushnumber(
				L,
				float( time - m_timeoffset ) / CLOCKS_PER_SEC );
			if( lua_pcall( L, 2, 0, 0 ) != 0 )
			{
				*error = lua_retrieveerror( L );
			}
			m_timeoffset = time;
			m_tpscounter += 1;
			time = clock();
			if( time > m_tpstime )
			{
				m_tps = m_tpscounter;
				m_tpscounter = 0;
				m_tpstime = time + CLOCKS_PER_SEC;
			}
			marker();
		// )
		}
	}

	void Window::setshape( graphics::Shape* nv )
	{
		m_display.setshape( nv );
	}

	WindowInfo const* Window::windowinfo()
	{
		return &m_info;
	}

	void Window::finish()
	{
		m_finished = true;
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
