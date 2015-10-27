#include "clearshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics {
	bool ClearShape::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}
	
	ClearShape::ClearShape() {
	}
	
	ClearShape::~ClearShape() {
	}
	
	void ClearShape::paint( IDirect3DDevice9* device ) {
		float color[ 4 ];
		getcolor( color );
		checkerror( device->Clear(
			0, 0, D3DCLEAR_TARGET,
			argb8( color ), 0, 0 ) );
	}
	
	extern "C" {
		ClearShape* graphics_clearshape_new() noexcept {
			CBASE_PROTECT(
				return new ClearShape();
			)
		}
	}
}
