#pragma once

#include <renderer-state/resource.hpp>
#include <utility>

namespace graphics
{
	namespace Context
	{
		std::atomic< size_t > DrawnFrame( 1 );
		MPSCQueue< method_t > Deferred;
		void* CurrentVertexDeclaration;
		void* CurrentVertexBuffer;

		void defer( void( *func )( void* ), void* ud, RefObject* target )
		{
			Deferred.push( func, ud, target );
		}

		void rundeferred()
		{
			method_t method;
			while( Deferred.pop( &method ) )
			{
				method.func( method.ud );
			}
		}
	}
}
