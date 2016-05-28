#include <renderer-gles/context.hpp>
#include <renderer-gles/resource.hpp>

namespace graphics
{
	namespace Context
	{
		std::atomic< size_t > DrawnFrame( 0 );
		utils::MPSCQueue< Resource* > Dead;
		// void* CurrentVertexDeclaration;
		// void* CurrentVertexBuffer;

		void markdead( Resource* res )
		{
			Dead.push( res );
		}

		void cleanup()
		{
			Resource* res;
			while( Dead.pop( &res ) )
			{
				delete res;
			}
		}
	}
}
