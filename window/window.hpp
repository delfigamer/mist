#ifndef WINDOW_WINDOW_HPP__
#define WINDOW_WINDOW_HPP__ 1

#include "asyncluastate.hpp"
#include <utils/configset.hpp>
#include <utils/ref.hpp>
#include <utils/console.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <graphics-d3d/display.hpp>
#include <windows.h>
#include <windowsx.h>
#elif defined(__ANDROID__)
#include <graphics-gles/display.hpp>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#else
#error "Unsupported target platform"
#endif
#include <atomic>
#include <ctime>

namespace window
{
	struct WindowInfo
	{
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
		char const* cmdline;
		bool acceleratorinput;
		bool pointinput;
		bool keyboardinput;
		bool silent;
	};

	struct WindowCreationData
	{
#if defined( CON_TARGET )
#elif defined( _WIN32 ) || defined( _WIN64 )
		HINSTANCE hInstance;
#elif defined(__ANDROID__)
		android_app* app;
#endif
		utils::String cmdline;
	};

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
#elif defined(__ANDROID__)
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
		utils::String m_cmdline;
		graphics::Display m_display;
		AsyncLuaState m_lstate;
		bool m_lstateready;
		bool m_silent;
		WindowInfo m_info;
		int m_fpscounter;
		int m_tpscounter;
		int m_fps;
		int m_tps;
		clock_t m_timeoffset;
		clock_t m_fpstime;
		clock_t m_tpstime;
		bool m_finished;
		utils::SingletonRef< utils::ConsoleClass > m_console;
		void initialize();
		void finalize();
		void paint();
		void pointevent(
			char const* event, int point, int x, int y );
		void keyevent( char const* event, int key );
		void charevent( char const* event, int ch );
		void notifyevent( char const* event );
		void initlstate();
		void closelstate();
		void ticklstate( lua_State* L, utils::String* error );

	public:
		Window() = delete;
		explicit Window( WindowCreationData const& wcd );
		Window( Window const& ) = delete;
		Window( Window&& ) = delete;
		~Window();

		int mainloop();
		void setshape( graphics::Shape* nv );
		WindowInfo const* windowinfo();
		void finish();
	};

	bool window_window_setshape(
		Window* window, graphics::Shape* shape ) noexcept;
	window::WindowInfo const* window_window_getinfo(
		Window* window ) noexcept;
	bool window_window_finish( Window* window ) noexcept;
}

#endif
