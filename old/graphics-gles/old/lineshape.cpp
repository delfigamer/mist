#include "lineshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics {
	bool LineShape::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}
	
	LineShape::LineShape() :
		m_pos{ 0, 0, 0, 0, 0, 0 } {
	}
	
	LineShape::~LineShape() {
	}
	
	void LineShape::paint() {
// 		std::lock_guard< Monitor > _lock_guard( *this );
// 		GLfloat pos[ 6 ];
// 		GLfloat color[ 8 ];
// 		getpos( pos );
// 		getcolor( color );
// 		for( int i = 0; i < 4; i++ ) {
// 			color[ i + 4 ] = color[ i ];
// 		}
// 		glEnableClientState( GL_VERTEX_ARRAY );	checkerror();
// 		glEnableClientState( GL_COLOR_ARRAY );	checkerror();
// 		glVertexPointer( 3, GL_FLOAT, 0, pos );	checkerror();
// 		glColorPointer( 4, GL_FLOAT, 0, color );	checkerror();
// 		glDrawArrays( GL_LINES, 0, 2 );	checkerror();
// 		glDisableClientState( GL_VERTEX_ARRAY );	checkerror();
// 		glDisableClientState( GL_COLOR_ARRAY );	checkerror();
	}
	
	void LineShape::getpos( GLfloat* pos ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		for( int i = 0; i < 6; i++ ) {
			pos[ i ] = m_pos[ i ];
		}
	}
	
	void LineShape::setpos( GLfloat const* pos ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		for( int i = 0; i < 6; i++ ) {
			m_pos[ i ] = pos[ i ];
		}
	}
	
	extern "C" {
		LineShape* graphics_lineshape_new() noexcept {
			CBASE_PROTECT(
// 				return new LineShape();
				cbase_seterror( "Line shapes are not supported" );
				return 0;
			)
		}
		
		bool graphics_lineshape_getpos( LineShape* shape, GLfloat* pos ) noexcept {
			CBASE_PROTECT(
				shape->getpos( pos );
				return 1;
			)
		}
		
		bool graphics_lineshape_setpos( LineShape* shape, GLfloat const* pos ) noexcept {
			CBASE_PROTECT(
				shape->setpos( pos );
				return 1;
			)
		}
	}
}
