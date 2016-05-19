#include <renderer-d3d9/context.hpp>
#include <renderer-d3d9/resource.hpp>

namespace graphics
{
	namespace Context
	{
		std::atomic< size_t > DrawnFrame( 0 );
		IDirect3D9* D3D = 0;
		IDirect3DDevice9* Device = 0;
		utils::MPSCQueue< Resource* > Dead;
		void* CurrentVertexDeclaration;
		void* CurrentVertexBuffer;

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
