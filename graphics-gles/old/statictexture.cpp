#include "statictexture.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	int log2( uint32_t i ) {
		static int const log2_16[] = {
			0,	1,	2,	2,	3,	3,	3,	3,	4,	4,	4,	4,	4,	4,	4,	4 };
		int r = -1;
		if( i & 0xffff0000 ) {
			r += 16;
			i >>= 16;
		}
		if( i & 0xff00 ) {
			r += 8;
			i >>= 8;
		}
		if( i & 0xf0 ) {
			r += 4;
			i >>= 4;
		}
		r += log2_16[ i ];
		return r;
	}

	void StaticTexture::update() {
		if( !m_data ) {
			return;
		}
		uint8_t* data = m_data->m_chars;
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_width,
			m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );	checkerror();
// 		if( glGenerateMipmap ) {
			glGenerateMipmap( GL_TEXTURE_2D ); checkerror();
// 		} else {
// 			int width = m_width >> 1;
// 			int height = m_height >> 1;
// 			int widthlog = log2( width );
// 			int miplevel = 1;
// 			while( width > 0 && height > 0 ) {
// 				for( int y = 0; y < height; y++ ) {
// 					for( int x = 0; x < width; x++ ) {
// 						uint8_t* target = data + ( ( ( y << widthlog ) + x ) << 2 );
// 						int x2 = x << 1;
// 						int y4 = y << 2;
// 						uint8_t* p1 = data + ( ( ( y4 << widthlog ) + x2 ) << 2 );
// 						uint8_t* p2 = data + ( ( ( y4 << widthlog ) + x2 + 1 ) << 2 );
// 						uint8_t* p3 = data + ( ( ( ( y4 + 2 ) << widthlog ) + x2 ) << 2 );
// 						uint8_t* p4 = data + ( ( ( ( y4 + 2 ) << widthlog ) + x2 + 1 ) << 2 );
// 						for( int ch = 0; ch < 4; ++ch ) {
// 							target[ ch ] = (
// 								uint16_t( p1[ ch ] ) +
// 								uint16_t( p2[ ch ] ) +
// 								uint16_t( p3[ ch ] ) +
// 								uint16_t( p4[ ch ] ) ) >> 2;
// 						}
// 					}
// 				}
// 				glTexImage2D( GL_TEXTURE_2D, miplevel, GL_RGBA, width,
// 					height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );	checkerror();
// 				width >>= 1;
// 				height >>= 1;
// 				widthlog -= 1;
// 				miplevel += 1;
// 			}
// 			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );	checkerror();
// 			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, miplevel );	checkerror();
// 		}
		m_data = nullptr;
	}

	bool StaticTexture::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	StaticTexture::StaticTexture() :
		m_data( nullptr ) ,
		m_width( 0 ) ,
		m_height( 0 ) {
	}

	StaticTexture::~StaticTexture() {
	}

	void StaticTexture::assign( utils::StringInstance* data, int width, int height ) {
		if( ( width & ( width - 1 ) ) != 0 || ( height & ( height - 1 ) ) != 0 ) {
			throw std::runtime_error( "only power-of-two textures are supported" );
		}
		m_data = data;
		m_width = width;
		m_height = height;
	}

	extern "C" {
		StaticTexture* graphics_statictexture_new() noexcept {
			CBASE_PROTECT(
				return new StaticTexture();
			)
		}

		bool graphics_statictexture_assign( StaticTexture* st, utils::StringInstance* data, int width, int height ) noexcept {
			CBASE_PROTECT(
				st->assign( data, width, height );
				return 1;
			)
		}
	}
}
