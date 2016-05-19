#include "sprite.hpp"
#include "common.hpp"
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
	
	void Sprite::setposition( float x, float y ) {
		lock_t _lock( m_mutex );
		m_data.pos_x = x;
		m_data.pos_y = y;
	}
	
	void Sprite::setextent( float x1, float y1, float x2, float y2 ) {
		lock_t _lock( m_mutex );
		m_data.ext_x1 = x1;
		m_data.ext_y1 = y1;
		m_data.ext_x2 = x2;
		m_data.ext_y2 = y2;
	}
	
	void Sprite::settexregion1( float x1, float y1, float x2, float y2 ) {
		lock_t _lock( m_mutex );
		m_data.tex1_x1 = x1;
		m_data.tex1_y1 = y1;
		m_data.tex1_x2 = x2;
		m_data.tex1_y2 = y2;
	}
	
	void Sprite::settexregion2( float x1, float y1, float x2, float y2 ) {
		lock_t _lock( m_mutex );
		m_data.tex2_x1 = x1;
		m_data.tex2_y1 = y1;
		m_data.tex2_x2 = x2;
		m_data.tex2_y2 = y2;
	}
	
	void Sprite::setcolor( float const* color ) {
		lock_t _lock( m_mutex );
		m_data.color = argb8( color );
	}
	
	bool Sprite::operator<( Sprite const& other ) const {
		int lo = m_order.load( std::memory_order_relaxed );
		int ro = other.m_order.load( std::memory_order_relaxed );
		if( lo < ro ) {
			return true;
		} else if( lo > ro ) {
			return false;
		} else {
			return m_gorder.load( std::memory_order_relaxed ) < other.m_gorder.load( std::memory_order_relaxed );
		}
	}
	
	extern "C" {
		bool graphics_sprite_setposition( Sprite* sprite, float x, float y ) noexcept {
		CBASE_PROTECT(
			sprite->setposition( x, y );
			return 1;
		)
		}
		
		bool graphics_sprite_setextent( Sprite* sprite, float x1, float y1, float x2, float y2 ) noexcept {
		CBASE_PROTECT(
			sprite->setextent( x1, y1, x2, y2 );
			return 1;
		)
		}
		
		bool graphics_sprite_settexregion1( Sprite* sprite, float x1, float y1, float x2, float y2 ) noexcept {
		CBASE_PROTECT(
			sprite->settexregion1( x1, y1, x2, y2 );
			return 1;
		)
		}
		
		bool graphics_sprite_settexregion2( Sprite* sprite, float x1, float y1, float x2, float y2 ) noexcept {
		CBASE_PROTECT(
			sprite->settexregion2( x1, y1, x2, y2 );
			return 1;
		)
		}
		
		bool graphics_sprite_setcolor( Sprite* sprite, float const* color ) noexcept {
		CBASE_PROTECT(
			sprite->setcolor( color );
			return 1;
		)
		}
	}
}
