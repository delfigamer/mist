#include <renderer-d3d9/interface.hpp>
#include <renderer-d3d9/methodlist.hpp>
#include <renderer-d3d9/luainit.hpp>
#include <renderer-d3d9/display.hpp>

#include <renderer-state/interface-cpp.hpp>

namespace graphics
{
	extern "C" EXPORT void renderer_connect( window::WindowInfo* info )
	{
		WindowInfo = info;
		info->renderer_methodlist = &renderer_d3d9_methodlist;
		info->renderer_module = &luainit;
	}
}
