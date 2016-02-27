#ifndef WINDOW_WINDOW_HPP__
#define WINDOW_WINDOW_HPP__ 1

#include "methodlist.hpp"
#include <utils/configset.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <lua/lua.hpp>
#include <ctime>

namespace graphics
{
	class Shape;
}

namespace window
{
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
*/

	struct WindowCreationData
	{
		utils::String cmdline;
	};

	R_CLASS( name = window )
	class Window
	{
	private:
		utils::ConfigSet m_mainconfig;
		utils::String m_cmdline;
		lua_State* m_lstate;
		WindowInfo m_info;
		void initialize();
		void initlstate();

	public:
		Window() = delete;
		explicit Window( WindowCreationData const& wcd );
		Window( Window const& ) = delete;
		Window( Window&& ) = delete;
		~Window();

		int mainloop();
		R_METHOD() static void finish() noexcept { }
	};
}

#endif
