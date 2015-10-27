#include "sprite.hpp"
#include <utils/cbase.hpp>

namespace graphics {
	std::atomic< int > Sprite::s_order( 0 );
	
	Sprite::Sprite( int order ) :
		m_gorder( s_order.fetch_add( 1, std::memory_order_relaxed ) ) ,
		m_order( order ) ,
		m_data{ 0 } {
	}
	
	Sprite::~Sprite() {
	}
	
	void Sprite::getdata( Sprite::Data* data ) {
		lock_t _lock( m_mutex );
		*data = m_data;
	}
	
	void Sprite::setposition( GLfloat x, GLfloat y ) {
		lock_t _lock( m_mutex );
		m_data.pos_x = x;
		m_data.pos_y = y;
	}
	
	void Sprite::setextent( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
		lock_t _lock( m_mutex );
		m_data.ext_x1 = x1;
		m_data.ext_y1 = y1;
		m_data.ext_x2 = x2;
		m_data.ext_y2 = y2;
	}
	
	void Sprite::settexregion( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
		lock_t _lock( m_mutex );
		m_data.tex_x1 = x1;
		m_data.tex_y1 = y1;
		m_data.tex_x2 = x2;
		m_data.tex_y2 = y2;
	}
	
	bool Sprite::operator<( Sprite const& other ) const {
		if( m_order.load( std::memory_order_relaxed ) < other.m_order.load( std::memory_order_relaxed ) ) {
			return true;
		} else {
			return m_gorder.load( std::memory_order_relaxed ) < other.m_gorder.load( std::memory_order_relaxed );
		}
	}
	
	extern "C" {
		bool graphics_sprite_setposition( Sprite* sprite, GLfloat x, GLfloat y ) noexcept {
		CBASE_PROTECT(
			sprite->setposition( x, y );
			return 1;
		)
		}
		
		bool graphics_sprite_setextent( Sprite* sprite, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) noexcept {
		CBASE_PROTECT(
			sprite->setextent( x1, y1, x2, y2 );
			return 1;
		)
		}
		
		bool graphics_sprite_settexregion( Sprite* sprite, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) noexcept {
		CBASE_PROTECT(
			sprite->settexregion( x1, y1, x2, y2 );
			return 1;
		)
		}
	}
}
