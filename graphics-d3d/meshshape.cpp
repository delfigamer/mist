#include "meshshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
// #include <stdexcept>

namespace graphics
{
	static int const blendsftable[] =
	{
		D3DBLEND_ONE,
		D3DBLEND_SRCALPHA,
		D3DBLEND_ONE,
		D3DBLEND_ONE,
	};
	static int const blenddftable[] =
	{
		D3DBLEND_ZERO,
		D3DBLEND_INVSRCALPHA,
		D3DBLEND_ONE,
		D3DBLEND_INVSRCCOLOR,
	};
	static int const blendoptable[] =
	{
		D3DBLENDOP_ADD,
		D3DBLENDOP_ADD,
		D3DBLENDOP_ADD,
		D3DBLENDOP_ADD,
	};

	void MeshShape::doadvance( IDirect3DDevice9* device, int framecount )
	{
		MeshData* md = m_meshdata;
		if( md )
		{
			md->advance( device, framecount );
		}
		Shader* sh = m_shader;
		if( sh )
		{
			sh->advance( device, framecount );
		}
	}

	MeshShape::MeshShape()
		: m_blendsf( D3DBLEND_ONE )
		, m_blenddf( D3DBLEND_ZERO )
		, m_blendop( D3DBLENDOP_ADD )
		, m_matrix
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		}
	{
	}

	MeshShape::~MeshShape()
	{
	}

	void MeshShape::paint( IDirect3DDevice9* device )
	{
		MeshData* md = m_meshdata;
		Shader* sh = m_shader;
		if( !md || !sh )
		{
			return;
		}
		int vertexcount;
		int indexcount;
		if( !md->bind( device, &vertexcount, &indexcount ) )
		{
			return;
		}
		sh->bind( device, m_textures );
		{
			lock_t lock( m_mutex );
			checkerror( device->SetRenderState( D3DRS_BLENDOP, m_blendop ) );
			checkerror( device->SetRenderState( D3DRS_SRCBLEND, m_blendsf ) );
			checkerror( device->SetRenderState( D3DRS_DESTBLEND, m_blenddf ) );
			checkerror( device->SetVertexShaderConstantF( 0, m_matrix.m[ 0 ], 4 ) );
			// checkerror( sh->m_vertexshaderconstants->SetMatrix( device, "worldviewmatrix", ( D3DXMATRIX* )&m_matrix ) );
		}
		checkerror( device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			vertexcount,
			0,
			indexcount / 3 ) );
	}

	void MeshShape::setmeshdata( MeshData* md )
	{
		m_meshdata = md;
	}

	void MeshShape::setshader( Shader* sh )
	{
		m_shader = sh;
	}

	void MeshShape::setblendmethod( int method )
	{
		lock_t lock( m_mutex );
		m_blendsf = TABLE_ASSERT( blendsftable, method, "blend method" );
		m_blenddf = TABLE_ASSERT( blenddftable, method, "blend method" );
		m_blendop = TABLE_ASSERT( blendoptable, method, "blend method" );
	}

	void MeshShape::settexture( int stage, Texture* texture )
	{
		if( stage < 0 || stage >= 8 )
		{
			throw std::runtime_error( "invalid stage index" );
		}
		m_textures[ stage ] = texture;
	}

	void MeshShape::setmatrix( float const* data )
	{
		lock_t lock( m_mutex );
		bool inverse = data[ 11 ] < 0;
		for( int row = 0; row < 4; ++row )
		{
			for( int col = 0; col < 4; ++col )
			{
				m_matrix.m[ row ][ col ] =
					inverse ?
					( - data[ row * 4 + col ] ) :
					data[ row * 4 + col ];
			}
		}
	}

	extern "C" {
		MeshShape* graphics_meshshape_new() noexcept
		{
		CBASE_PROTECT(
			return new MeshShape();
		)
		}

		bool graphics_meshshape_setmeshdata( MeshShape* shape, MeshData* md ) noexcept
		{
		CBASE_PROTECT(
			shape->setmeshdata( md );
			return 1;
		)
		}

		bool graphics_meshshape_setshader( MeshShape* shape, Shader* sh ) noexcept
		{
		CBASE_PROTECT(
			shape->setshader( sh );
			return 1;
		)
		}

		bool graphics_meshshape_setblendmethod( MeshShape* shape, int method ) noexcept
		{
		CBASE_PROTECT(
			shape->setblendmethod( method );
			return 1;
		)
		}

		bool graphics_meshshape_settexture( MeshShape* shape, int stage, Texture* texture ) noexcept
		{
		CBASE_PROTECT(
			shape->settexture( stage, texture );
			return 1;
		)
		}

		bool graphics_meshshape_setmatrix( MeshShape* shape, float const* data ) noexcept
		{
		CBASE_PROTECT(
			shape->setmatrix( data );
			return 1;
		)
		}
	}
}
