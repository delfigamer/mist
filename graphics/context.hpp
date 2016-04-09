#ifndef GRAPHICS_CONTEXT_HPP__
#define GRAPHICS_CONTEXT_HPP__ 1

#include <utils/mpscqueue.hpp>
#include <d3d9.h>
#include <atomic>
#include <cstdint>

namespace graphics
{
	class Resource;

	namespace Context
	{
		extern std::atomic< size_t > DrawnFrame;
		extern IDirect3D9* D3D;
		extern IDirect3DDevice9* Device;
		extern utils::MPSCQueue< Resource* > Dead;
		extern void* CurrentVertexDeclaration;
		extern void* CurrentVertexBuffer;

		void markdead( Resource* res );
		void cleanup();
	};
}

#endif
