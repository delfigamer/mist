#ifndef WINDOW_WINDOW_HPP__
#define WINDOW_WINDOW_HPP__ 1

#include "event.hpp"
#include <client-main/methodlist.hpp>
#include <utils/mpscqueue.hpp>
#include <utils/configset.hpp>
#include <utils/ref.hpp>
#include <utils/console.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <graphics/display.hpp>
#include <windows.h>
#include <windowsx.h>
#elif defined(__ANDROID__)
#include <graphics/display.hpp>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#else
#error "Unsupported target platform"
#endif
#include <common.hpp>
#include <lua/lua.hpp>
#include <thread>
#include <atomic>
#include <ctime>

namespace window
{
	class Window;

R_EMIT( target = ffi_end )
	struct WindowInfo
	{
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
R_END()
/*
R_EMIT( target = ffi_end )
		configset const* configset;
R_END()
*/
		utils::ConfigSet const* configset;
R_EMIT( target = ffi_end )
		bool acceleratorinput;
		bool pointinput;
		bool keyboardinput;
		bool silent;
		methodlist_t const* methodlist;
R_END()
/*
R_EMIT( target = ffi_end )
		window* window;
R_END()
*/
		Window* window;
R_EMIT( target = ffi_end )
	};
R_END()
/*
R_EMIT( target = ffi_end )
	typedef struct WindowInfo windowinfo_t;
R_END()

R_EMIT( target = lua_beforeclasses )
local windowinfo = package.loaded['host.info']
windowinfo = ffi.cast('windowinfo_t const*', windowinfo)
package.loaded['host.info'] = windowinfo
local methodlist = windowinfo.methodlist
R_END()

R_EMIT( target = lua_end )
package.loaded['host.window'] = windowinfo.window
R_END()
*/

	struct WindowCreationData
	{
#if defined( CON_TARGET )
#elif defined( _WIN32 ) || defined( _WIN64 )
		HINSTANCE hInstance;
#elif defined( __ANDROID__ )
		android_app* app;
#endif
		utils::String cmdline;
	};

	R_CLASS( name = window )
	class Window
	{
#if defined( _WIN32 ) || defined( _WIN64 )
	private:
		static wchar_t const* ClassName;
		static wchar_t const* WindowCaption;
		static DWORD const WindowStyle =
			WS_OVERLAPPED |
			WS_CAPTION |
			WS_SYSMENU |
			WS_BORDER |
			WS_MINIMIZEBOX;
		WNDCLASSW m_wndclass;
		HWND m_hwnd;
		bool m_pointstate[ 4 ];
		bool m_terminated;

	public:
		friend LRESULT CALLBACK GlobalWindowProc(
			HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	private:
		LRESULT WindowProc(
			UINT uMsg, WPARAM wParam, LPARAM lParam );
		void pointmove( int point, bool newstate, LPARAM lParam );
#elif defined( __ANDROID__ )
	private:
		android_app* m_app;
		ASensorManager* m_sensorManager;
		ASensor const* m_accelerometerSensor;
		ASensorEventQueue* m_sensorEventQueue;

	public:
		friend int32_t g_handle_input(
			android_app* app, AInputEvent* event );
		friend void g_handle_cmd( android_app* app, int32_t cmd );
	private:
		int32_t handle_input( AInputEvent* event );
		void handle_cmd( int32_t cmd );
		void handle_sensor( ASensorEvent* event );
#endif
	private:
		utils::ConfigSet m_mainconfig;
		graphics::Display m_display;
		std::thread m_luathread;
		utils::MPSCQueue< Event > m_eventqueue;
		bool m_silent;
		WindowInfo m_info;
		int m_fpscounter;
		int m_tpscounter;
		int m_fps;
		int m_tps;
		clock_t m_timeoffset;
		clock_t m_fpstime;
		clock_t m_tpstime;
		bool m_finishrequired;
		std::atomic< bool > m_finished;

		void initialize();
		void finalize();
		void paint();
		void luathreadfunc();
		void initlstate();
		void closelstate();
		void pushevent( int name,
			int a1 = 0, int a2 = 0, int a3 = 0, int a4 = 0 );

	public:
		Window() = delete;
		explicit Window( WindowCreationData const& wcd );
		Window( Window const& ) = delete;
		Window( Window&& ) = delete;
		~Window();

		int mainloop();
		R_METHOD() void setshape( graphics::Shape* nv );
		R_METHOD() bool popevent( Event* event );
	};
}

#endif
