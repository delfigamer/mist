#include <renderer-gles/primitiveshape.hpp>
#include <renderer-gles/context.hpp>
#include <renderer-gles/common.hpp>
#include <renderer-gles/gl2.hpp>
#include <cstring>

namespace graphics
{
	static int const blendoptable[] =
	{
		GL_FUNC_ADD,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
	};
	static int const blendftable[] =
	{
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
	};
	static int const typetable[] =
	{
		GL_POINTS,
		GL_LINE_STRIP,
		GL_LINES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_TRIANGLES,
	};
	static int const pfactortable[] =
	{
		1,
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
		Ref< VertexBuffer > vb = m_vertexbuffer.getref();
		if( vb )
		{
			vb->advance();
		}
		// IndexBuffer* ib = m_indexbuffer;
		// if( ib )
		// {
			// ib->advance();
		// }
		Ref< Program > pr = m_program.getref();
		if( pr )
		{
			pr->advance();
		}
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
	{
		memset( &m_matrix, 0, sizeof( m_matrix ) );
		m_matrix[ 0 ] = m_matrix[ 5 ] = m_matrix[ 10 ] = m_matrix[ 15 ] = 1;
		m_type.store( RANGE_ASSERT(
				type, 0, int( primitivetype::invalid ), "primitive type" ),
			std::memory_order_release );
	}

	PrimitiveShape::~PrimitiveShape()
	{
	}

	void PrimitiveShape::paint()
	{
		Ref< VertexBuffer > vb = m_vertexbuffer.getref();
		if( !vb )
		{
			return;
		}
		// IndexBuffer* ib = m_indexbuffer;
		Ref< Program > pr = m_program.getref();
		if( !pr )
		{
			return;
		}
		size_t vertexcount;
		VertexDeclaration* vdecl;
		if( !vb->bind( &vertexcount, &vdecl ) )
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
		int worldmatrixpos;
		if( !pr->bind( &worldmatrixpos ) )
		{
			return;
		}
		int ptype = m_type.load( std::memory_order_acquire );
		glBlendEquation( m_blendop );
		checkerror();
		glBlendFunc( m_blendsf, m_blenddf );
		checkerror();
		if( worldmatrixpos != -1 )
		{
			lock_t lock( m_mutex );
			glUniformMatrix4fv( worldmatrixpos, 1, false, m_matrix );
			checkerror();
		}
		glDrawArrays( typetable[ ptype ], 0, GLsizei( vertexcount ) );
		checkerror();
	}

	void PrimitiveShape::setvertexbuffer( VertexBuffer* value )
	{
		m_vertexbuffer.assign( value );
	}

	// void PrimitiveShape::setindexbuffer( IndexBuffer* value )
	// {
		// m_indexbuffer = value;
	// }

	void PrimitiveShape::setprogram( Program* value )
	{
		m_program.assign( value );
	}

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
				m_matrix[ row * 4 + col ] =
					// inverse ?
					// ( - value[ row * 4 + col ] ) :
					value[ row * 4 + col ];
			}
		}
	}
}
