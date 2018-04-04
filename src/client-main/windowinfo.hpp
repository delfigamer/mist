#pragma once

#include <common.hpp>

namespace window
{
	class [[ r::external, r::name( "window" ) ]] Window;

	struct [[ r::class, r::name( "windowinfo" ) ]] WindowInfo
	{
		[[ r::field, r::const ]] Window* window;
		[[ r::field, r::const ]] bool acceleratorinput;
		[[ r::field, r::const ]] bool pointinput;
		[[ r::field, r::const ]] bool keyboardinput;
		[[ r::field, r::const ]] bool silent;
		void const* renderer_methodlist;
		void const* renderer_module;
		[[ r::field, r::const ]] int width;
		[[ r::field, r::const ]] int height;
		[[ r::field, r::const ]] float texelsoffset;
		[[ r::field, r::const ]] float texeltoffset;
	};
}
