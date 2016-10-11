#pragma once

#include <common/mpscqueue.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <atomic>
#include <cstdint>

namespace graphics
{
	class Resource;

	namespace Context
	{
		struct method_t
		{
			void( *func )( void* );
			void* ud;
			Ref< RefObject > target;

			method_t() = default;
			method_t( void( *func )( void* ), void* ud, RefObject* target )
				: func( func )
				, ud( ud )
				, target( target )
			{
			}
		};

		extern std::atomic< size_t > DrawnFrame;
		extern MPSCQueue< method_t > Deferred;
		extern void* CurrentVertexDeclaration;
		extern void* CurrentVertexBuffer;

		void defer( void( *func )( void* ), void* ud, RefObject* target );
		void rundeferred();
	};
}
