#include <graphics/primitiveshape.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
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

	void PrimitiveShape::doadvance()
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
		Program* pr = m_program;
		if( pr )
		{
			pr->advance();
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

	PrimitiveShape::PrimitiveShape()
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

	PrimitiveShape::~PrimitiveShape()
	{
	}

	void PrimitiveShape::paint()
	{
		VertexBuffer* vb = m_vertexbuffer;
		IndexBuffer* ib = m_indexbuffer;
		Program* pr = m_program;
		if( !vb || !pr )
		{
			return;
		}
		int vertexcount;
		if( !vb->bind( &vertexcount ) )
		{
			return;
		}
		int indexcount;
		if( ib )
		{
			if( !ib->bind( &indexcount ) )
			{
				return;
			}
		}
		int worldmatrixpos;
		if( !pr->bind( &worldmatrixpos ) )
		{
			return;
		}
		{
			lock_t lock( m_mutex );
			checkerror( Context::Device->SetRenderState(
				D3DRS_BLENDOP, m_blendop ) );
			checkerror( Context::Device->SetRenderState(
				D3DRS_SRCBLEND, m_blendsf ) );
			checkerror( Context::Device->SetRenderState(
				D3DRS_DESTBLEND, m_blenddf ) );
			checkerror( Context::Device->SetVertexShaderConstantF(
				worldmatrixpos, m_matrix.m[ 0 ], 4 ) );
		}
		checkerror( Context::Device->DrawIndexedPrimitive(
			D3DPRIMITIVETYPE( m_type ),
			0,
			0,
			vertexcount,
			0,
			indexcount / 3 ) );
	}

	void PrimitiveShape::setvertexbuffer( VertexBuffer* value )
	{
		m_vertexbuffer = value;
	}

	void PrimitiveShape::setindexbuffer( IndexBuffer* value )
	{
		m_indexbuffer = value;
	}

	void PrimitiveShape::setprogram( Program* value )
	{
		m_program = value;
	}

	// void PrimitiveShape::settexture( int index, Texture* texture )
	// {
		// if( index < 0 || index >= 8 )
		// {
			// throw std::runtime_error( "invalid texture index" );
		// }
		// m_textures[ index ] = texture;
	// }

	void PrimitiveShape::settype( int value )
	{
		lock_t lock( m_mutex );
		m_type = TABLE_ASSERT( typetable, value, "primitive type" );
	}

	void PrimitiveShape::setblend( int sfactor, int dfactor, int method )
	{
		lock_t lock( m_mutex );
		sfactor = TABLE_ASSERT( blendftable, sfactor, "blend factor" );
		dfactor = TABLE_ASSERT( blendftable, dfactor, "blend factor" );
		method = TABLE_ASSERT( blendoptable, method, "blend method" );
		m_blendsf = sfactor;
		m_blenddf = dfactor;
		m_blendop = method;
	}

	void PrimitiveShape::setmatrix( float const* value )
	{
		lock_t lock( m_mutex );
		// bool inverse = value[ 11 ] < 0;
		for( int row = 0; row < 4; ++row )
		{
			for( int col = 0; col < 4; ++col )
			{
				m_matrix.m[ row ][ col ] =
					// inverse ?
					// ( - value[ row * 4 + col ] ) :
					value[ row * 4 + col ];
			}
		}
	}
}
