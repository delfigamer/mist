#include "context.hpp"
#include "resource.hpp"

namespace graphics
{
	namespace Context
	{
		std::atomic< int > DrawnFrame( 0 );
		IDirect3D9* D3D = 0;
		IDirect3DDevice9* Device = 0;
		utils::MPSCQueue< Resource* > Dead;

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
