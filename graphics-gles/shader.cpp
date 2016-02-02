#include "shader.hpp"
#include "common.hpp"
#include <utils/databuffer.hpp>
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <cstring>

namespace graphics
{
	static bool advanceshader(
		GLuint* shader, utils::String* shaderdata,
		GLenum shadertype, bool* success )
	{
		if( *shaderdata )
		{
			*success = false;
			if( !*shader )
			{
				*shader = glCreateShader( shadertype );
				checkerror();
			}
			glShaderSource(
				*shader,
				1,
				( char const** )&shaderdata->m_payload->m_data,
				&shaderdata->m_payload->m_length );
			checkerror();
			glCompileShader( *shader );
			checkerror();
			GLint status;
			glGetShaderiv( *shader, GL_COMPILE_STATUS, &status );
			checkerror();
			*success = status;
			if( !status )
			{
				GLint length;
				glGetShaderiv( *shader, GL_INFO_LOG_LENGTH, &length );
				checkerror();
				utils::Ref< utils::DataBuffer > logbuf =
					utils::DataBuffer::create( length, length, 0 );
				glGetShaderInfoLog(
					*shader, logbuf->m_capacity, 0,
					( char* )logbuf->m_data );
				checkerror();
				LOG( "shader compilation error: %s", logbuf->m_data );
			}
			*shaderdata = 0;
			return true;
		}
		else
		{
			return false;
		}
	}

	void Shader::doadvance( int framecount )
	{
		lock_t _lock_guard( m_mutex );
		for( int i = 0; i < 8; ++i )
		{
			Texture* tex = m_textures[ i ];
			if( tex ) {
				tex->advance( framecount );
			}
		}
		bool shaders_updated =
			advanceshader( &m_vertexshader, &m_vertexshaderdata,
				GL_VERTEX_SHADER, &m_validvs ) &&
			advanceshader( &m_pixelshader, &m_pixelshaderdata,
				GL_FRAGMENT_SHADER, &m_validps );
		if( shaders_updated )
		{
			m_validprogram = false;
			if( m_validvs && m_validps )
			{
				if( !m_program )
				{
					m_program = glCreateProgram();
					checkerror();
					glAttachShader( m_program, m_vertexshader );
					checkerror();
					glAttachShader( m_program, m_pixelshader );
					checkerror();
				}
				glLinkProgram( m_program );
				checkerror();
				GLint status;
				glGetProgramiv( m_program, GL_LINK_STATUS, &status );
				checkerror();
				m_validprogram = status;
				if( status )
				{
					m_wvmindex = glGetUniformLocation( m_program, "worldviewmatrix" );
				}
				else
				{
					GLint length;
					glGetProgramiv( m_program, GL_INFO_LOG_LENGTH, &length );
					checkerror();
					utils::Ref< utils::DataBuffer > logbuf =
						utils::DataBuffer::create( length, length, 0 );
					glGetProgramInfoLog(
						m_program, logbuf->m_capacity, 0,
						( char* )logbuf->m_data );
					checkerror();
					LOG( "program link error: %s", logbuf->m_data );
				}
			}
			if( m_validprogram )
			{
				char const* texnames = m_samplerlist.getchars();
				for( int index = 0; index < 8; ++index )
				{
					int scpos = strcspn( texnames, ";" );
					if( scpos > 0 )
					{
						if( scpos < 256 )
						{
							char name[ 256 ];
							memcpy( name, texnames, scpos );
							name[ scpos ] = 0;
							m_samplerindex[ index ] =
								glGetUniformLocation( m_program, name );
							checkerror();
							LOG( "%i %s -> %i",
								index, name, m_samplerindex[ index ] );
						}
						else
						{
							char* name = new char[ scpos + 1 ];
							memcpy( name, texnames, scpos );
							name[ scpos ] = 0;
							m_samplerindex[ index ] =
								glGetUniformLocation( m_program, name );
							LOG( "%i %s -> %i",
								index, name, m_samplerindex[ index ] );
							delete[] name;
						}
					}
					else
					{
						m_samplerindex[ index ] = -1;
						LOG( "%i -> -1", index );
					}
					if( texnames[ scpos ] )
					{
						texnames = texnames + scpos + 1;
					}
					else
					{
						texnames = texnames + scpos;
					}
				}
			}
		}
	}

	Shader::Shader()
		: m_vertexshader( 0 )
		, m_pixelshader( 0 )
		, m_program( 0 )
		, m_validvs( false )
		, m_validps( false )
		, m_validprogram( false )
	{
	}

	Shader::~Shader()
	{
		glDeleteProgram( m_program );
		glDeleteShader( m_vertexshader );
		glDeleteShader( m_pixelshader );
	}

	bool Shader::bind( utils::Ref< Texture > const* textures, int* wvmindex )
	{
		lock_t _lock_guard( m_mutex );
		if( !m_validprogram )
		{
			return false;
		}
		for( int i = 0; i < 8; ++i )
		{
			glUniform1i( m_samplerindex[ i ], i );
			checkerror();
			Texture* tex = textures[ i ];
			if( !tex )
			{
				tex = m_textures[ i ];
			}
			if( tex )
			{
				tex->bind( i );
			}
			else
			{
				glActiveTexture( GL_TEXTURE0 + i );
				checkerror();
				glBindTexture( GL_TEXTURE_2D, 0 );
				checkerror();
			}
		}
		glUseProgram( m_program );
		checkerror();
		*wvmindex = m_wvmindex;
		return true;
	}

	void Shader::settexture( int stage, Texture* texture )
	{
		TABLE_ASSERT( m_textures, stage, "stage" ) = texture;
	}

	void Shader::setshadersources(
		int format, char const* vert, char const* frag, char const* texnames )
	{
		if( format != ShaderFormat_GLSL_100 )
		{
			throw std::runtime_error( "unsupported shader format" );
		}
		lock_t _lock_guard( m_mutex );
		m_vertexshaderdata = vert;
		m_pixelshaderdata = frag;
		m_samplerlist = texnames;
	}

	Shader* graphics_shader_new() noexcept
	{
	CBASE_PROTECT(
		return new Shader();
	)
	}

	bool graphics_shader_settexture(
		Shader* sh, int stage, Texture* texture ) noexcept
	{
	CBASE_PROTECT(
		sh->settexture( stage, texture );
		return 1;
	)
	}

	bool graphics_shader_setshadersources(
		Shader* sh,
		int format,
		char const* vert, char const* frag, char const* texnames ) noexcept
	{
	CBASE_PROTECT(
		sh->setshadersources( format, vert, frag, texnames );
		return 1;
	)
	}
}
