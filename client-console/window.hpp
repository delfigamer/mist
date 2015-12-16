#ifndef WINDOW_WINDOW_HPP__
#define WINDOW_WINDOW_HPP__ 1

#include <utils/configset.hpp>
#include <utils/ref.hpp>
#include <utils/console.hpp>
#include <lua/lua.hpp>
#include <ctime>

namespace graphics
{
	class Shape;
}

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
		utils::String cmdline;
	};

	class Window
	{
	private:
		utils::ConfigSet m_mainconfig;
		utils::String m_cmdline;
		lua_State* m_lstate;
		WindowInfo m_info;
		utils::SingletonRef< utils::ConsoleClass > m_console;
		void initialize();
		void initlstate();

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
