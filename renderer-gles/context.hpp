#pragma once

#include <utils/mpscqueue.hpp>
#include <atomic>
#include <cstdint>

namespace graphics
{
	class Resource;

	namespace Context
	{
		extern std::atomic< size_t > DrawnFrame;
		extern utils::MPSCQueue< Resource* > Dead;
		// extern void* CurrentVertexDeclaration;
		// extern void* CurrentVertexBuffer;

		void markdead( Resource* res );
		void cleanup();
	};
}
