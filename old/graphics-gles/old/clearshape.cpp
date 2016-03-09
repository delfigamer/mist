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
	
	void ClearShape::paint() {
		std::lock_guard< Monitor > _lock_guard( *this );
		GLfloat color[ 4 ];
		getcolor( color );
		glClearColor( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
		glClear( GL_COLOR_BUFFER_BIT );
		checkerror();
	}
	
	extern "C" {
		ClearShape* graphics_clearshape_new() noexcept {
			CBASE_PROTECT(
				return new ClearShape();
			)
		}
	}
}
