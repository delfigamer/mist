#include <renderer-gles/program.hpp>
#include <renderer-gles/programtranslator.hpp>
#include <renderer-gles/context.hpp>
#include <renderer-gles/common.hpp>
#include <renderer-gles/gl2.hpp>
#include <cstring>

namespace graphics
{
	namespace
	{
		Ref< DataBuffer > strtodb( char const* str )
		{
			return DataBuffer::create( strlen( str ), str );
		}

		bool compileshader(
			DataBuffer* text,
			Ref< DataBuffer >* plog, unsigned int* psh,
			GLenum type )
		{
			*psh = glCreateShader( type );
			checkerror();
			char const* textarr[] = { ( char const* )text->m_data };
			glShaderSource( *psh, 1, textarr, 0 );
			checkerror();
			glCompileShader( *psh );
			checkerror();
			int status;
			glGetShaderiv( *psh, GL_COMPILE_STATUS, &status );
			checkerror();
			if( !status )
			{
				int length;
				glGetShaderiv( *psh, GL_INFO_LOG_LENGTH, &length );
				checkerror();
				*plog = DataBuffer::create( length );
				glGetShaderInfoLog(
					*psh, GLsizei( ( *plog )->m_capacity ),
					0, ( char* )( *plog )->m_data );
				checkerror();
				return false;
			}
			return true;
		}

		bool linkprogram(
			unsigned int vsh, unsigned int fsh,
			Ref< DataBuffer >* plog, unsigned int* pprogram )
		{
			*pprogram = glCreateProgram();
			checkerror();
			glAttachShader( *pprogram, vsh );
			checkerror();
			glAttachShader( *pprogram, fsh );
			checkerror();
			char attributestr[] = "attribute_x";
			for( int i = 0; i < 8; ++i )
			{
				attributestr[ 10 ] = '0' + i;
				glBindAttribLocation( *pprogram, i, attributestr );
				checkerror();
			}
			glLinkProgram( *pprogram );
			checkerror();
			int status;
			glGetProgramiv( *pprogram, GL_LINK_STATUS, &status );
			checkerror();
			if( status )
			{
				glValidateProgram( *pprogram );
				checkerror();
				glGetProgramiv( *pprogram, GL_VALIDATE_STATUS, &status );
				checkerror();
			}
			if( !status )
			{
				int length;
				glGetProgramiv( *pprogram, GL_INFO_LOG_LENGTH, &length );
				checkerror();
				*plog = DataBuffer::create( length );
				glGetProgramInfoLog(
					*pprogram, GLsizei( ( *plog )->m_capacity ),
					0, ( char* )( *plog )->m_data );
				checkerror();
				return false;
			}
			return true;
		}
	}

	void Program::doadvance()
	{
		if( m_ready.load( std::memory_order_relaxed ) )
		{
			return;
		}
		try
		{
			Ref< DataBuffer > source = m_source.detachref();
			if( source )
			{
				Ref< DataBuffer > vsrc;
				Ref< DataBuffer > fsrc;
				translateprogram( source, &vsrc, &fsrc );
				if( !compileshader(
					vsrc, &m_log, &m_vertexshader, GL_VERTEX_SHADER ) )
				{
					m_valid = false;
					goto end;
				}
				if( !compileshader(
					fsrc, &m_log, &m_fragmentshader, GL_FRAGMENT_SHADER ) )
				{
					m_valid = false;
					goto end;
				}
				if( !linkprogram(
					m_vertexshader, m_fragmentshader,
					&m_log, &m_shaderprogram ) )
				{
					m_valid = false;
					goto end;
				}
				m_matrixindex = glGetUniformLocation(
					m_shaderprogram, "worldviewmatrix" );
				checkerror();
			}
			else
			{
				throw std::runtime_error( "shader source or binary required" );
			}
			m_valid = true;
			if( m_cache )
			{
				if( !m_binary )
				{
					// not supported
				}
			}
			else
			{
				clearcache();
			}
		}
		catch( std::exception const& e )
		{
			m_log = strtodb( e.what() );
			m_valid = false;
		}
	end:
		m_ready.store( true, std::memory_order_release );
	}

	// Program::Program( DataBuffer* binary, bool cache )
		// : m_cache( cache )
		// , m_ready( false )
	// {
		// m_binary.assign( binary );
		// if( cache )
		// {
			// deferadvance();
		// }
	// }

	Program::Program( DataBuffer* source, bool cache, int )
		: m_cache( cache )
		, m_ready( false )
		, m_vertexshader( 0 )
		, m_fragmentshader( 0 )
		, m_shaderprogram( 0 )
		, m_matrixindex( -1 )
	{
		m_source.assign( source );
		if( cache )
		{
			deferadvance();
		}
	}

	Program::~Program()
	{
		if( m_shaderprogram )
		{
			glDeleteProgram( m_shaderprogram );
		}
		if( m_vertexshader )
		{
			glDeleteShader( m_vertexshader );
		}
		if( m_fragmentshader )
		{
			glDeleteShader( m_fragmentshader );
		}
	}

	bool Program::bind( int* pmatrixindex )
	{
		if( !m_valid || !m_shaderprogram )
		{
			return false;
		}
		glUseProgram( m_shaderprogram );
		checkerror();
		*pmatrixindex = m_matrixindex;
		return true;
	}

	void Program::clearcache()
	{
		m_binary = nullptr;
		m_log = nullptr;
	}
}
