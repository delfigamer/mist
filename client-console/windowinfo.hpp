#pragma once

#include <common.hpp>

struct client_console_methodlist_t;

namespace window
{
	class Window;

R_STRUCT( name = windowinfo_t )
	struct WindowInfo
	{
		Window* window;
		bool acceleratorinput;
		bool pointinput;
		bool keyboardinput;
		bool silent;
		client_console_methodlist_t const* client_methodlist;
		void const* renderer_methodlist;
		void const* renderer_module;
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
	};
/*
R_EMIT( target = lua_beforeclasses )
	local windowinfo = package.loaded['host.info']
	windowinfo = ffi.cast('struct windowinfo_t const*', windowinfo)
	package.loaded['host.info'] = windowinfo
	local methodlist = windowinfo.client_methodlist
R_END()

R_EMIT( target = lua_end )
	package.loaded['host.window'] = window:new(windowinfo.window)
R_END()
*/
}
