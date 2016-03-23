#include <graphics/primitiveshape.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
#include <utils/console.hpp>
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
	static int const pfactortable[] =
	{
		0,
		1,
		2,
		1,
		1,
		3,
	};
	static int const poffsettable[] =
	{
		0,
		1,
		0,
		2,
		2,
		0,
	};

	void PrimitiveShape::doadvance()
	{
		VertexBuffer* vb = m_vertexbuffer;
		if( vb )
		{
			vb->advance();
		}
		// IndexBuffer* ib = m_indexbuffer;
		// if( ib )
		// {
			// ib->advance();
		// }
		// Program* pr = m_program;
		// if( pr )
		// {
			// pr->advance();
		// }
		// for( int i = 0; i < 8; ++i )
		// {
			// Texture* t = m_textures[ i ];
			// if( t )
			// {
				// t->advance();
			// }
		// }
	}

	PrimitiveShape::PrimitiveShape(
		int type, int sourceblend, int destblend, int blendmethod )
		: m_blendsf( TABLE_ASSERT(
			blendftable, sourceblend, "source blend factor" ) )
		, m_blenddf( TABLE_ASSERT(
			blendftable, destblend, "destination blend factor" ) )
		, m_blendop( TABLE_ASSERT(
			blendoptable, blendmethod, "blend method" ) )
		, m_matrix{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		}
	{
		m_type.store( RANGE_ASSERT(
				type, 0, Primitive_Invalid, "primitive type" ),
			std::memory_order_release );
	}

	PrimitiveShape::~PrimitiveShape()
	{
	}

	void PrimitiveShape::paint()
	{
		VertexBuffer* vb = m_vertexbuffer;
		// IndexBuffer* ib = m_indexbuffer;
		// Program* pr = m_program;
		if( !vb )
		{
			return;
		}
		int vertexcount;
		if( !vb->bind( &vertexcount ) )
		{
			return;
		}
		// int indexcount;
		// if( ib )
		// {
			// if( !ib->bind( &indexcount ) )
			// {
				// return;
			// }
		// }
		// int worldmatrixpos;
		// if( !pr->bind( &worldmatrixpos ) )
		// {
			// return;
		// }
		int ptype = m_type.load( std::memory_order_acquire );
		checkerror( Context::Device->SetRenderState(
			D3DRS_BLENDOP, m_blendop ) );
		checkerror( Context::Device->SetRenderState(
			D3DRS_SRCBLEND, m_blendsf ) );
		checkerror( Context::Device->SetRenderState(
			D3DRS_DESTBLEND, m_blenddf ) );
		{
			lock_t lock( m_mutex );
			checkerror( Context::Device->SetTransform(
				D3DTS_WORLD, &m_matrix ) );
			// checkerror( Context::Device->SetVertexShaderConstantF(
				// worldmatrixpos, m_matrix.m[ 0 ], 4 ) );
		}
		checkerror( Context::Device->SetTextureStageState(
			0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ) );
		checkerror( Context::Device->SetTextureStageState(
			0, D3DTSS_COLORARG1, D3DTA_CONSTANT ) );
		checkerror( Context::Device->SetTextureStageState(
			0, D3DTSS_CONSTANT, 0xff0000ff ) );
		// checkerror( Context::Device->DrawIndexedPrimitive(
			// D3DPRIMITIVETYPE( m_type ),
			// 0,
			// 0,
			// vertexcount,
			// 0,
			// indexcount / 3 ) );
		checkerror( Context::Device->DrawPrimitive(
			D3DPRIMITIVETYPE( typetable[ ptype ] ),
			0,
			( vertexcount - poffsettable[ ptype ] ) / pfactortable[ ptype ] ) );
	}

	void PrimitiveShape::setvertexbuffer( VertexBuffer* value )
	{
		m_vertexbuffer = value;
	}

	// void PrimitiveShape::setindexbuffer( IndexBuffer* value )
	// {
		// m_indexbuffer = value;
	// }

	// void PrimitiveShape::setprogram( Program* value )
	// {
		// m_program = value;
	// }

	// void PrimitiveShape::settexture( int index, Texture* texture )
	// {
		// if( index < 0 || index >= 8 )
		// {
			// throw std::runtime_error( "invalid texture index" );
		// }
		// m_textures[ index ] = texture;
	// }

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
