#include "statictexture.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
// 	int log2( uint32_t i ) {
// 		static int const log2_16[] = {
// 			0,	1,	2,	2,	3,	3,	3,	3,	4,	4,	4,	4,	4,	4,	4,	4 };
// 		int r = -1;
// 		if( i & 0xffff0000 ) {
// 			r += 16;
// 			i >>= 16;
// 		}
// 		if( i & 0xff00 ) {
// 			r += 8;
// 			i >>= 8;
// 		}
// 		if( i & 0xf0 ) {
// 			r += 4;
// 			i >>= 4;
// 		}
// 		r += log2_16[ i ];
// 		return r;
// 	}

	void StaticTexture::update( IDirect3DDevice9* device ) {
		if( !m_data ) {
			return;
		}
		utils::Ref< utils::StringInstance > datainst;
		int width;
		int height;
		{
			std::lock_guard< Monitor > _lock_guard( *this );
			datainst = std::move( m_data );
			width = m_width;
			height = m_height;
		}
		uint8_t* data = datainst->m_chars;
		checkerror( device->CreateTexture(
			width, height, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_texture, 0 ) );
		D3DLOCKED_RECT rect;
		checkerror( m_texture->LockRect( 0, &rect, 0, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
		try {
			for( int y = 0; y < m_height; ++y ) {
				uint8_t* trow = ( uint8_t* )rect.pBits + rect.Pitch * y;
				uint8_t* drow = data + y * ( m_width << 2 );
				for( int x = 0; x < m_width; ++x ) {
					uint8_t* tp = trow + ( x << 2 );
					uint8_t* dp = drow + ( x << 2 );
					tp[ 0 ] = dp[ 2 ];
					tp[ 1 ] = dp[ 1 ];
					tp[ 2 ] = dp[ 0 ];
					tp[ 3 ] = dp[ 3 ];
				}
			}
		} catch( ... ) {
			m_texture->UnlockRect( 0 );
		}
		checkerror( m_texture->UnlockRect( 0 ) );
		m_texture->GenerateMipSubLevels();
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
		std::lock_guard< Monitor > _lock_guard( *this );
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
