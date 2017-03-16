#pragma once

#include <common/ref.hpp>
#include <d3d9.h>

#include <renderer-state/context.hpp>

namespace graphics
{
	namespace Context
	{
		extern Ref< IDirect3D9 > D3D;
		extern Ref< IDirect3DDevice9 > Device;
	};
}
