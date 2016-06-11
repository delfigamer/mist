#pragma once

#include <renderer-state/context.hpp>

#include <d3d9.h>

namespace graphics
{
	namespace Context
	{
		extern IDirect3D9* D3D;
		extern IDirect3DDevice9* Device;
	};
}
