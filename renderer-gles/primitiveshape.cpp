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

#define SOURCE( ... ) #__VA_ARGS__
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

		static char const* vsh_source[] = { SOURCE(
			precision highp float;
			attribute vec4 attribute_0;
			attribute vec4 attribute_1;
			uniform mat4 worldmatrix;
			varying vec4 color;

			void main()
			{
				color = attribute_1;
				gl_Position = worldmatrix * attribute_0;
			}
		) };
		static char const* fsh_source[] = { SOURCE(
			precision highp float;
			varying vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		) };

		if( !m_vsh )
		{
			m_vsh = glCreateShader( GL_VERTEX_SHADER );
			checkerror();
			glShaderSource( m_vsh, 1, vsh_source, 0 );
			checkerror();
			glCompileShader( m_vsh );
			checkerror();
			int status;
			glGetShaderiv( m_vsh, GL_COMPILE_STATUS, &status );
			checkerror();
			if( !status )
			{
				int length;
				glGetShaderiv( m_vsh, GL_INFO_LOG_LENGTH, &length );
				checkerror();
				auto log = utils::DataBuffer::create( length, length, 0 );
				glGetShaderInfoLog(
					m_vsh, GLsizei( log->m_capacity ), 0, ( char* )log->m_data );
				checkerror();
				throw utils::StrException(
					"VShader compilation failed: %s", ( char const* )log->m_data );
			}
		}
		if( !m_fsh )
		{
			m_fsh = glCreateShader( GL_FRAGMENT_SHADER );
			checkerror();
			glShaderSource( m_fsh, 1, fsh_source, 0 );
			checkerror();
			glCompileShader( m_fsh );
			checkerror();
			int status;
			glGetShaderiv( m_fsh, GL_COMPILE_STATUS, &status );
			checkerror();
			if( !status )
			{
				int length;
				glGetShaderiv( m_fsh, GL_INFO_LOG_LENGTH, &length );
				checkerror();
				auto log = utils::DataBuffer::create( length, length, 0 );
				glGetShaderInfoLog(
					m_fsh, GLsizei( log->m_capacity ), 0, ( char* )log->m_data );
				checkerror();
				throw utils::StrException(
					"FShader compilation failed: %s", ( char const* )log->m_data );
			}
		}
		if( !m_program )
		{
			m_program = glCreateProgram();
			checkerror();
			glAttachShader( m_program, m_vsh );
			checkerror();
			glAttachShader( m_program, m_fsh );
			checkerror();
			glBindAttribLocation( m_program, 0, "attribute_0" );
			checkerror();
			glBindAttribLocation( m_program, 1, "attribute_1" );
			checkerror();
			glLinkProgram( m_program );
			checkerror();
			int status;
			glGetProgramiv( m_program, GL_LINK_STATUS, &status );
			checkerror();
			if( !status )
			{
				int length;
				glGetProgramiv( m_program, GL_INFO_LOG_LENGTH, &length );
				checkerror();
				auto log = utils::DataBuffer::create( length, length, 0 );
				glGetProgramInfoLog(
					m_program, GLsizei( log->m_capacity ), 0, ( char* )log->m_data );
				checkerror();
				throw utils::StrException(
					"Program link failed: %s", ( char const* )log->m_data );
			}
			m_matrixindex = glGetUniformLocation( m_program, "worldmatrix" );
			checkerror();
		}
	}

	PrimitiveShape::PrimitiveShape(
		int type, int sourceblend, int destblend, int blendmethod )
		: m_blendsf( TABLE_ASSERT(
			blendftable, sourceblend, "source blend factor" ) )
		, m_blenddf( TABLE_ASSERT(
			blendftable, destblend, "destination blend factor" ) )
		, m_blendop( TABLE_ASSERT(
			blendoptable, blendmethod, "blend method" ) )
#if !defined( _MSC_VER )
		, m_matrix{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		}
#endif
		, m_vsh( 0 )
		, m_fsh( 0 )
		, m_program( 0 )
		, m_matrixindex( -1 )
	{
#if defined( _MSC_VER )
		memset( &m_matrix, 0, sizeof( m_matrix ) );
		m_matrix[ 0 ] = m_matrix[ 5 ] = m_matrix[ 10 ] = m_matrix[ 15 ] = 1;
#endif
		m_type.store( RANGE_ASSERT(
				type, 0, int( primitivetype::invalid ), "primitive type" ),
			std::memory_order_release );
	}

	PrimitiveShape::~PrimitiveShape()
	{
		if( m_vsh )
		{
			glDeleteShader( m_vsh );
		}
		if( m_fsh )
		{
			glDeleteShader( m_fsh );
		}
		if( m_program )
		{
			glDeleteProgram( m_program );
		}
	}

	void PrimitiveShape::paint()
	{
		VertexBuffer* vb = m_vertexbuffer;
		// IndexBuffer* ib = m_indexbuffer;
		// Program* pr = m_program;
		if( !vb || !m_program )
		{
			return;
		}
		size_t vertexcount;
		VertexDeclaration* vdecl;
		if( !vb->bind( &vertexcount, &vdecl ) )
		{
			return;
		}
		glUseProgram( m_program );
		checkerror();
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
		glBlendEquation( m_blendop );
		checkerror();
		glBlendFunc( m_blendsf, m_blenddf );
		checkerror();
		if( m_matrixindex != -1 )
		{
			lock_t lock( m_mutex );
			glUniformMatrix4fv( m_matrixindex, 1, false, m_matrix );
		}
		// checkerror( Context::Device->DrawIndexedPrimitive(
			// D3DPRIMITIVETYPE( m_type ),
			// 0,
			// 0,
			// vertexcount,
			// 0,
			// indexcount / 3 ) );
		glDrawArrays( typetable[ ptype ], 0, GLsizei( vertexcount ) );
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
				m_matrix[ row * 4 + col ] =
					// inverse ?
					// ( - value[ row * 4 + col ] ) :
					value[ row * 4 + col ];
			}
		}
	}
}
