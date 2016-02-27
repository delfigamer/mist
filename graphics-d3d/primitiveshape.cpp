#include "meshshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics
{
	static int const blendoptable[] =
	{
		D3DBLENDOP_ADD,
		D3DBLENDOP_SUBTRACT,
		D3DBLENDOP_REVSUBTRACT,
	};
	static int const blendftable[] =
	{
		D3DBLEND_ZERO,
		D3DBLEND_ONE,
		D3DBLEND_SRCCOLOR,
		D3DBLEND_INVSRCCOLOR,
		D3DBLEND_DESTCOLOR,
		D3DBLEND_INVDESTCOLOR,
		D3DBLEND_SRCALPHA,
		D3DBLEND_INVSRCALPHA,
		D3DBLEND_DESTALPHA,
		D3DBLEND_INVDESTALPHA,
	};
	static int const typetable[] =
	{
		D3DPT_POINTLIST,
		D3DPT_LINESTRIP,
		D3DPT_LINELIST,
		D3DPT_TRIANGLESTRIP,
		D3DPT_TRIANGLEFAN,
		D3DPT_TRIANGLELIST,
	};

	void MeshShape::doadvance()
	{
		VertexBuffer* vd = m_vertexbuffer;
		if( vd )
		{
			vd->advance();
		}
		IndexBuffer* id = m_indexbuffer;
		if( id )
		{
			id->advance();
		}
		Shader* sh = m_shader;
		if( sh )
		{
			sh->advance();
		}
		for( int i = 0; i < 8; ++i )
		{
			Texture* t = m_textures[ i ];
			if( t )
			{
				t->advance();
			}
		}
	}

	MeshShape::MeshShape()
		: m_type( D3DPT_POINTLIST )
		, m_blendsf( D3DBLEND_ONE )
		, m_blenddf( D3DBLEND_ZERO )
		, m_blendop( D3DBLENDOP_ADD )
		, m_matrix{
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

	void MeshShape::paint()
	{
		VertexBuffer* vb = m_vertexbuffer;
		IndexBuffer* ib = m_indexbuffer;
		Shader* sh = m_shader;
		if( !vb || !sh )
		{
			return;
		}
		int vertexcount;
		if( !vb->bind( &vertexcount ) )
		{
			return;
		}
		if( ib )
		{
			int indexcount;
			if( !ib->bind( &indexcount ) )
			{
				return;
			}
		}
		sh->bind( device, m_textures );
		{
			lock_t lock( m_mutex );
			checkerror( device->SetRenderState( D3DRS_BLENDOP, m_blendop ) );
			checkerror( device->SetRenderState( D3DRS_SRCBLEND, m_blendsf ) );
			checkerror( device->SetRenderState( D3DRS_DESTBLEND, m_blenddf ) );
			checkerror( device->SetVertexShaderConstantF(
				0, m_matrix.m[ 0 ], 4 ) );
		}
		checkerror( device->DrawIndexedPrimitive(
			m_type,
			0,
			0,
			vertexcount,
			0,
			indexcount / 3 ) );
	}

	void MeshShape::setvertexbuffer( VertexBuffer* value )
	{
		m_vertexbuffer = value;
	}

	void MeshShape::setindexbuffer( InexBuffer* value )
	{
		m_indexbuffer = value;
	}

	void MeshShape::setshader( Shader* value )
	{
		m_shader = value;
	}

	void MeshShape::settexture( int index, Texture* texture )
	{
		if( index < 0 || index >= 8 )
		{
			throw std::runtime_error( "invalid texture index" );
		}
		m_textures[ index ] = texture;
	}

	void MeshShape::settype( int value )
	{
		lock_t lock( m_mutex );
		m_type = TABLE_ASSERT( typetable, value, "primitive type" );
	}

	void MeshShape::setblend( int sfactor, int dfactor, int method )
	{
		lock_t lock( m_mutex );
		sfactor = TABLE_ASSERT( blendftable, sfactor, "blend factor" );
		dfactor = TABLE_ASSERT( blendftable, dfactor, "blend factor" );
		method = TABLE_ASSERT( blendoptable, method, "blend method" );
		m_blendsf = sfactor;
		m_blenddf = dfactor;
		m_blendop = method;
	}

	void MeshShape::setmatrix( float const* value )
	{
		lock_t lock( m_mutex );
		// bool inverse = data[ 11 ] < 0;
		for( int row = 0; row < 4; ++row )
		{
			for( int col = 0; col < 4; ++col )
			{
				m_matrix.m[ row ][ col ] =
					// inverse ?
					// ( - data[ row * 4 + col ] ) :
					value[ row * 4 + col ];
			}
		}
	}

	MeshShape* graphics_meshshape_new() noexcept
	{
	CBASE_PROTECT(
		return new MeshShape();
	)
	}

	bool graphics_meshshape_setvertexbuffer(
		MeshShape* shape, VertexBuffer* value ) noexcept
	{
	CBASE_PROTECT(
		shape->setvertexbuffer( value );
		return 1;
	)
	}

	bool graphics_meshshape_setindexbuffer(
		MeshShape* shape, IndexBuffer* value ) noexcept
	{
	CBASE_PROTECT(
		shape->setindexbuffer( value );
		return 1;
	)
	}

	bool graphics_meshshape_setshader(
		MeshShape* shape, Shader* value ) noexcept
	{
	CBASE_PROTECT(
		shape->setshader( value );
		return 1;
	)
	}

	bool graphics_meshshape_settexture(
		MeshShape* shape, int index, Texture* texture ) noexcept
	{
	CBASE_PROTECT(
		shape->settexture( index, texture );
		return 1;
	)
	}

	bool graphics_meshshape_settype(
		MeshShape* shape, int value ) noexcept
	{
	CBASE_PROTECT(
		shape->settype( value );
		return 1;
	)
	}

	bool graphics_meshshape_setblend(
		MeshShape* shape, int sfactor, int dfactor, int method ) noexcept
	{
	CBASE_PROTECT(
		shape->setblend( sfactor, dfactor, method );
		return 1;
	)
	}

	bool graphics_meshshape_setmatrix(
		MeshShape* shape, float const* value ) noexcept
	{
	CBASE_PROTECT(
		shape->setmatrix( value );
		return 1;
	)
	}
}
