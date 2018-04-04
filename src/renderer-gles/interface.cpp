#include <renderer-gles/interface.hpp>
#include <renderer-gles/methodlist.hpp>
#include <renderer-gles/luainit.hpp>
#include <renderer-gles/display.hpp>

#include <renderer-state/interface-cpp.hpp>

namespace graphics
{
	extern "C" EXPORT void renderer_connect( window::WindowInfo* info )
	{
		WindowInfo = info;
		info->renderer_methodlist = &renderer_gles_methodlist;
		info->renderer_module = &luainit;
	}
}
