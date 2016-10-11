#include <renderer-d3d9/context.hpp>

#include <renderer-state/context-cpp.hpp>

namespace graphics
{
	namespace Context
	{
		Ref< IDirect3D9 > D3D;
		Ref< IDirect3DDevice9 > Device;
	}
}
