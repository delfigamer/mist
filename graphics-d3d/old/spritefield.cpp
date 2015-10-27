#include "spritefield.hpp"
#include "common.hpp"
#include <utils/profile.hpp>
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	static int const blendsftable[] = {
		D3DBLEND_ONE,
		D3DBLEND_SRCALPHA,
		D3DBLEND_ONE,
		D3DBLEND_ONE,
	};
	static int const blenddftable[] = {
		D3DBLEND_ZERO,
		D3DBLEND_INVSRCALPHA,
		D3DBLEND_ONE,
		D3DBLEND_INVSRCCOLOR,
	};
	static int const blendoptable[] = {
		D3DBLENDOP_ADD,
		D3DBLENDOP_ADD,
		D3DBLENDOP_ADD,
		D3DBLENDOP_ADD,
	};

	static void fillvertexquad( Sprite::Data const& sdata, SpriteField::Vertex* vertices ) {
		vertices[ 0 ].pos_x = sdata.pos_x - sdata.ext_x1 - sdata.ext_x2;
		vertices[ 0 ].pos_y = sdata.pos_y - sdata.ext_y1 - sdata.ext_y2;
		vertices[ 0 ].pos_z = 0;
		vertices[ 0 ].tex1_x = sdata.tex1_x1;
		vertices[ 0 ].tex1_y = sdata.tex1_y1;
		vertices[ 0 ].tex2_x = sdata.tex2_x1;
		vertices[ 0 ].tex2_y = sdata.tex2_y1;
		vertices[ 0 ].color = sdata.color;

		vertices[ 1 ].pos_x = sdata.pos_x + sdata.ext_x1 - sdata.ext_x2;
		vertices[ 1 ].pos_y = sdata.pos_y + sdata.ext_y1 - sdata.ext_y2;
		vertices[ 1 ].pos_z = 0;
		vertices[ 1 ].tex1_x = sdata.tex1_x2;
		vertices[ 1 ].tex1_y = sdata.tex1_y1;
		vertices[ 1 ].tex2_x = sdata.tex2_x2;
		vertices[ 1 ].tex2_y = sdata.tex2_y1;
		vertices[ 1 ].color = sdata.color;

		vertices[ 2 ].pos_x = sdata.pos_x + sdata.ext_x1 + sdata.ext_x2;
		vertices[ 2 ].pos_y = sdata.pos_y + sdata.ext_y1 + sdata.ext_y2;
		vertices[ 2 ].pos_z = 0;
		vertices[ 2 ].tex1_x = sdata.tex1_x2;
		vertices[ 2 ].tex1_y = sdata.tex1_y2;
		vertices[ 2 ].tex2_x = sdata.tex2_x2;
		vertices[ 2 ].tex2_y = sdata.tex2_y2;
		vertices[ 2 ].color = sdata.color;

		vertices[ 3 ].pos_x = sdata.pos_x - sdata.ext_x1 + sdata.ext_x2;
		vertices[ 3 ].pos_y = sdata.pos_y - sdata.ext_y1 + sdata.ext_y2;
		vertices[ 3 ].pos_z = 0;
		vertices[ 3 ].tex1_x = sdata.tex1_x1;
		vertices[ 3 ].tex1_y = sdata.tex1_y2;
		vertices[ 3 ].tex2_x = sdata.tex2_x1;
		vertices[ 3 ].tex2_y = sdata.tex2_y2;
		vertices[ 3 ].color = sdata.color;
	}

	bool SpriteField::SpriteComparer::operator()( Sprite* lhs, Sprite* rhs ) const {
		return *lhs < *rhs;
	}

	void SpriteField::doadvance( IDirect3DDevice9* device, int framecount ) {
		std::lock_guard< Monitor > _lock_guard( *this );
	PROFILE( "advance",
		{
			lock_t _lock_guard( m_mutex );
			while( !m_newsprites.empty() ) {
				m_sprites.insert( m_newsprites.top() );
				m_newsprites.pop();
			}
		}
		if( !m_vertexdeclaration ) {
			static D3DVERTEXELEMENT9 const VDElements[] = {
				{ 0, offsetof( Vertex, pos_x ), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, offsetof( Vertex, tex1_x ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 0, offsetof( Vertex, tex2_x ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
				{ 0, offsetof( Vertex, color ), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				D3DDECL_END()
			};
			checkerror( device->CreateVertexDeclaration(
				VDElements,
				&m_vertexdeclaration ) );
		}
		PixelProgram* pp = m_pixelprogram;
		if( pp ) {
			pp->advance( device, framecount );
		}
		for( int i = 0; i < 8; ++i ) {
			Texture* tex = m_textures[ i ];
			if( tex ) {
				tex->advance( device, framecount );
			}
		}
		int count = m_sprites.size();
		if( count > m_buffersize ) {
			RELEASE( m_vertexbuffer );
			RELEASE( m_indexbuffer );
			if( count > 0 ) {
				m_buffersize = ( count + 0xff ) & 0xffffff00;
				checkerror( device->CreateVertexBuffer(
					( sizeof( Vertex ) * 4 ) * m_buffersize,
					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
					0,
					D3DPOOL_DEFAULT,
					&m_vertexbuffer,
					0 ) );
				checkerror( device->CreateIndexBuffer(
					( sizeof( uint16_t ) * 6 ) * m_buffersize,
					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
					D3DFMT_INDEX16,
					D3DPOOL_DEFAULT,
					&m_indexbuffer,
					0 ) );
			}
		}
		if( count > 0 ) {
			Vertex* vertices;
			uint16_t* indices;
			checkerror( m_vertexbuffer->Lock( 0, 0, ( void** )&vertices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
			try {
				checkerror( m_indexbuffer->Lock( 0, 0, ( void** )&indices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
				try {
					auto it = m_sprites.begin();
					Sprite::Data data;
					for( int i = 0; i < count; ++i ) {
						( *it )->getdata( &data );
						fillvertexquad( data, vertices + ( i << 2 ) );
						uint16_t* qi = indices + i * 6;
						qi[ 0 ] = ( i << 2 );
						qi[ 1 ] = ( i << 2 ) | 1;
						qi[ 2 ] = ( i << 2 ) | 2;
						qi[ 3 ] = ( i << 2 );
						qi[ 4 ] = ( i << 2 ) | 2;
						qi[ 5 ] = ( i << 2 ) | 3;
						++it;
					}
				} catch( ... ) {
					m_indexbuffer->Unlock();
					throw;
				}
				checkerror( m_indexbuffer->Unlock() );
			} catch( ... ) {
				m_vertexbuffer->Unlock();
				throw;
			}
			checkerror( m_vertexbuffer->Unlock() );
		}
	)
	}

	bool SpriteField::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	SpriteField::SpriteField() :
		m_buffersize( 0 ) ,
		m_vertexbuffer( 0 ) ,
		m_indexbuffer( 0 ) ,
		m_vertexdeclaration( 0 ) ,
		m_blendsf( D3DBLEND_ONE ) ,
		m_blenddf( D3DBLEND_ZERO ) ,
		m_blendop( D3DBLENDOP_ADD ) ,
		m_matrix{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 } {
	}

	SpriteField::~SpriteField() {
		RELEASE( m_vertexbuffer );
		RELEASE( m_indexbuffer );
		RELEASE( m_vertexdeclaration );
	}

	void SpriteField::paint( IDirect3DDevice9* device ) {
		if( !m_vertexbuffer || !m_indexbuffer || !m_vertexdeclaration ) {
			return;
		}
		std::lock_guard< Monitor > _lock_guard( *this );
	PROFILE( "paint",
		int spritecount = m_sprites.size();
		if( spritecount == 0 ) {
			return;
		}
		PixelProgram* pp = m_pixelprogram;
		if( !pp ) {
			return;
		}
		pp->bind( device, m_textures );
		{
			lock_t _lock_guard( m_modemutex );
			checkerror( device->SetRenderState( D3DRS_BLENDOP, m_blendop ) );
			checkerror( device->SetRenderState( D3DRS_SRCBLEND, m_blendsf ) );
			checkerror( device->SetRenderState( D3DRS_DESTBLEND, m_blenddf ) );
			checkerror( device->SetTransform( D3DTS_WORLD, &m_matrix ) );
		}
		checkerror( device->SetStreamSource( 0, m_vertexbuffer, 0, sizeof( Vertex ) ) );
		checkerror( device->SetVertexDeclaration( m_vertexdeclaration ) );
		checkerror( device->SetIndices( m_indexbuffer ) );
		checkerror( device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, 0, 0, spritecount * 4, 0, spritecount * 2 ) );
	)
	}

	void SpriteField::setpixelprogram( PixelProgram* pixelprogram ) {
		m_pixelprogram = pixelprogram;
	}

	Sprite* SpriteField::newsprite( int order ) {
	PROFILE( "newsprite",
		lock_t _lock_guard( m_mutex );
		Sprite* sprite = new Sprite( order );
		m_newsprites.push( sprite );
		return sprite;
	)
	}

	void SpriteField::removesprite( Sprite* sprite ) {
		bool success;
		{
			std::lock_guard< Monitor > _lock_guard( *this );
			success = m_sprites.erase( sprite ) != 0;
		}
		if( !success ) {
			throw std::runtime_error( "passed sprite does not belong to this field" );
		}
		delete sprite;
	}

	void SpriteField::setblendmethod( int method ) {
		lock_t _lock_guard( m_modemutex );
		m_blendsf = TABLE_ASSERT( blendsftable, method, "blend method" );
		m_blenddf = TABLE_ASSERT( blenddftable, method, "blend method" );
		m_blendop = TABLE_ASSERT( blendoptable, method, "blend method" );
	}

	void SpriteField::settexture( int stage, Texture* texture ) {
		if( stage < 0 || stage >= 8 ) {
			throw std::runtime_error( "invalid stage index" );
		}
		m_textures[ stage ] = texture;
	}

	void SpriteField::setmatrix( float const* data ) {
		lock_t _lock_guard( m_modemutex );
		bool inverse = data[ 11 ] < 0;
		for( int row = 0; row < 4; ++row ) {
			for( int col = 0; col < 4; ++col ) {
				m_matrix.m[ row ][ col ] =
					inverse ?
					( - data[ ( row << 2 ) + col ] ) :
					data[ ( row << 2 ) + col ];
			}
		}
	}

	extern "C" {
		SpriteField* graphics_spritefield_new() noexcept {
		CBASE_PROTECT(
			return new SpriteField();
		)
		}

		bool graphics_spritefield_setpixelprogram( SpriteField* sf, PixelProgram* pixelprogram ) noexcept {
		CBASE_PROTECT(
			sf->setpixelprogram( pixelprogram );
			return 1;
		)
		}

		Sprite* graphics_spritefield_newsprite( SpriteField* sf, int order ) noexcept {
		CBASE_PROTECT(
			return sf->newsprite( order );
		)
		}

		bool graphics_spritefield_removesprite( SpriteField* sf, Sprite* sprite ) noexcept {
		CBASE_PROTECT(
			sf->removesprite( sprite );
			return 1;
		)
		}

		bool graphics_spritefield_setblendmethod( SpriteField* sf, int method ) noexcept {
		CBASE_PROTECT(
			sf->setblendmethod( method );
			return 1;
		)
		}

		bool graphics_spritefield_settexture( SpriteField* sf, int stage, Texture* texture ) noexcept {
		CBASE_PROTECT(
			sf->settexture( stage, texture );
			return 1;
		)
		}

		bool graphics_spritefield_setmatrix( SpriteField* sf, float const* data ) noexcept {
		CBASE_PROTECT(
			sf->setmatrix( data );
			return 1;
		)
		}
	}
}
