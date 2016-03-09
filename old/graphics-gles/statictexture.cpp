#include "statictexture.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics
{
	// int log2( uint32_t i ) {
		// static int const log2_16[] = {
			// 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };
		// int r = -1;
		// if( i & 0xffff0000 ) {
			// r += 16;
			// i >>= 16;
		// }
		// if( i & 0xff00 ) {
			// r += 8;
			// i >>= 8;
		// }
		// if( i & 0xf0 ) {
			// r += 4;
			// i >>= 4;
		// }
		// r += log2_16[ i ];
		// return r;
	// }

	void StaticTexture::update()
	{
		// int format;
		utils::Ref< utils::DataBuffer > datainst;
		int width;
		int height;
		{
			lock_t lock( m_mutex );
			// format = m_format;
			datainst = m_data;
			width = m_width;
			height = m_height;
			m_data = nullptr;
		}
		if( !datainst || width == 0 || height == 0 )
		{
			return;
		}
		if( !m_texture )
		{
			glGenTextures( 1, &m_texture );
			checkerror();
		}
		uint8_t* data = datainst->m_data;
		glBindTexture( GL_TEXTURE_2D, m_texture );
		checkerror();
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			GL_RGBA,
			width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE,
			data );
		checkerror();
		glGenerateMipmap( GL_TEXTURE_2D );
		checkerror();
	}

	StaticTexture::StaticTexture()
		: m_format( 0 )
		, m_data( nullptr )
		, m_width( 0 )
		, m_height( 0 )
	{
	}

	StaticTexture::~StaticTexture()
	{
	}

	void StaticTexture::assign(
		int format, utils::DataBuffer* data, int width, int height )
	{
		if( ( width & ( width - 1 ) ) != 0 || ( height & ( height - 1 ) ) != 0 )
		{
			throw std::runtime_error( "only power-of-two textures are supported" );
		}
		if( format != 0 )
		{
			throw std::runtime_error( "invalid format" );
		}
		lock_t lock( m_mutex );
		m_format = format;
		m_data = data;
		m_width = width;
		m_height = height;
	}

	StaticTexture* graphics_statictexture_new() noexcept
	{
	CBASE_PROTECT(
		return new StaticTexture();
	)
	}

	bool graphics_statictexture_assign(
		StaticTexture* st,
		int format, utils::DataBuffer* data, int width, int height) noexcept
	{
	CBASE_PROTECT(
		st->assign( format, data, width, height );
		return 1;
	)
	}
}
