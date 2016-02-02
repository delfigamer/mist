#include "texture.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics
{
	static int const minfiltertable[] = {
		GL_NEAREST,
		GL_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR,
	};
	
	static int const magfiltertable[] = {
		GL_NEAREST,
		GL_LINEAR,
	};
	
	static int const wrapmodetable[] = {
		GL_REPEAT,
		GL_CLAMP_TO_EDGE,
	};

	void Texture::doadvance( int framecount )
	{
		update();
	}

	Texture::Texture()
		: m_texture( 0 )
		, m_minfilter( GL_LINEAR )
		, m_magfilter( GL_LINEAR_MIPMAP_LINEAR )
		, m_wraps( GL_REPEAT )
		, m_wrapt( GL_REPEAT )
	{
	}

	Texture::~Texture()
	{
		glDeleteTextures( 1, &m_texture );
	}

	void Texture::bind( int sampler )
	{
		glActiveTexture( GL_TEXTURE0 + sampler );
		checkerror();
		if( m_texture )
		{
			glBindTexture( GL_TEXTURE_2D, m_texture );
			checkerror();
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wraps );
			checkerror();
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapt );
			checkerror();
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minfilter );
			checkerror();
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magfilter );
			checkerror();
		}
		else
		{
			glBindTexture( GL_TEXTURE_2D, 0 );
			checkerror();
		}
	}

// 	void Texture::unbind() {
// 	}

	void Texture::setminfilter( int value )
	{
		if( value < 0 || value >= MinFilter_Invalid )
		{
			throw std::runtime_error( "unknown min filter value" );
		}
		m_minfilter = minfiltertable[ value ];
	}

	void Texture::setmagfilter( int value )
	{
		if( value < 0 || value >= MagFilter_Invalid )
		{
			throw std::runtime_error( "unknown mag filter value" );
		}
		m_magfilter = magfiltertable[ value ];
	}

	void Texture::setwrapmode( int ws, int wt )
	{
		if( ws < 0 || ws >= WrapMode_Invalid ||
			wt < 0 || wt >= WrapMode_Invalid )
		{
			throw std::runtime_error( "unknown wrap mode value" );
		}
		m_wraps = wrapmodetable[ ws ];
		m_wrapt = wrapmodetable[ wt ];
	}

	bool graphics_texture_setminfilter( Texture* t, int value ) noexcept
	{
	CBASE_PROTECT(
		t->setminfilter( value );
		return 1;
	)
	}

	bool graphics_texture_setmagfilter( Texture* t, int value ) noexcept
	{
	CBASE_PROTECT(
		t->setmagfilter( value );
		return 1;
	)
	}

	bool graphics_texture_setwrapmode( Texture* t, int ws, int wt ) noexcept
	{
	CBASE_PROTECT(
		t->setwrapmode( ws, wt );
		return 1;
	)
	}
}
