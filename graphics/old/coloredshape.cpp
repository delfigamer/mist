#include "coloredshape.hpp"
#include <utils/cbase.hpp>

namespace graphics {
	bool ColoredShape::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	ColoredShape::ColoredShape() :
		m_color{ 0, 0, 0, 0 } {
	}

	ColoredShape::ColoredShape( float const* color ) :
		m_color{ color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] } {
	}

	ColoredShape::~ColoredShape() {
	}

	void ColoredShape::getcolor( float* color ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		for( int i = 0; i < 4; i++ ) {
			color[ i ] = m_color[ i ];
		}
	}

	void ColoredShape::setcolor( float const* color ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		for( int i = 0; i < 4; i++ ) {
			m_color[ i ] = color[ i ];
		}
	}

	extern "C" {
		bool graphics_coloredshape_getcolor( ColoredShape* shape, float* color ) noexcept {
			CBASE_PROTECT(
				shape->getcolor( color );
				return 1;
			)
		}

		bool graphics_coloredshape_setcolor( ColoredShape* shape, float const* color ) noexcept {
			CBASE_PROTECT(
				shape->setcolor( color );
				return 1;
			)
		}
	}
}
