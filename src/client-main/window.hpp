#pragma once

#include <client-main/windowinfo.hpp>
#include <client-main/event.hpp>
#include <common/mpscqueue.hpp>
#include <common/ref.hpp>
#include <utils/console.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#include <windowsx.h>
#elif defined(__ANDROID__)
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

namespace graphics
{
	class [[ r::external, r::name( "shape" ) ]] Shape;
	class Display;
}

namespace window
{
	struct WindowCreationData
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		HINSTANCE hInstance;
#elif defined( __ANDROID__ )
		android_app* app;
#endif
	};

	class [[ r::class, r::name( "window" ) ]] Window
	{
#if defined( _WIN32 ) || defined( _WIN64 )
	private:
		typedef void( *renderer_connect_t )( window::WindowInfo* info );
		typedef bool( *renderer_display_create_t )(
			HWND hwnd, graphics::Display** display );
		typedef bool( *renderer_display_destroy_t )( graphics::Display* display );
		typedef bool( *renderer_display_paint_t )( graphics::Display* display );
		typedef bool( *renderer_display_setshape_t )(
			graphics::Display* display, graphics::Shape* shape );

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
		HMODULE m_renderermodule;
		renderer_connect_t m_renderer_connect;
		renderer_display_create_t m_renderer_display_create;
		renderer_display_destroy_t m_renderer_display_destroy;
		renderer_display_paint_t m_renderer_display_paint;
		renderer_display_setshape_t m_renderer_display_setshape;

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
		graphics::Display* m_display;
		std::thread m_luathread;
		MPSCQueue< Event > m_eventqueue;
		bool m_silent;
		WindowInfo m_info;
		int m_fpscounter;
		int m_fps;
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
		void pushevent( EventName name,
			int a1 = 0, int a2 = 0, int a3 = 0, int a4 = 0 );

	public:
		Window() = delete;
		explicit Window( WindowCreationData const& wcd );
		Window( Window const& ) = delete;
		Window( Window&& ) = delete;
		~Window();

		int mainloop();
		[[ r::method ]] void setshape( graphics::Shape* nv );
		[[ r::method ]] bool popevent( Event* event [[ r::required ]] );
	};
}
