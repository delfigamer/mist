#include <graphics/statictexture.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
#include <utils/cbase.hpp>
#include <stdexcept>
#include <atomic>

namespace graphics
{
	static D3DFORMAT const formattable[] = {
		D3DFMT_A8B8G8R8,
		D3DFMT_A16B16G16R16,
	};
	static int const texelsizetable[] = {
		4,
		8,
	};

	void StaticTexture::doadvance()
	{
		int width = m_width;
		int height = m_height;
		checkerror( Context::Device->CreateTexture(
			width, height,
			m_levelcount, 0,
			formattable[ m_format ], D3DPOOL_MANAGED, &m_texture, 0 ) );
		for( int i = 0; i < m_levelcount; ++i )
		{
			D3DLOCKED_RECT rect;
			checkerror( m_texture->LockRect(
				i, &rect, 0, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
			try
			{
				utils::Ref< utils::DataBuffer > level = std::move( m_levels[ i ] );
				if( !level )
				{
					break;
				}
				uint8_t* data = level->m_data;
				int rstride = width * texelsizetable[ m_format ];
				for( int y = 0; y < height; ++y )
				{
					uint8_t* trow = ( uint8_t* )rect.pBits + rect.Pitch * y;
					uint8_t* drow = data + y * width;
					memcpy( trow, drow, rstride );
				}
			}
			catch( ... )
			{
				m_texture->UnlockRect( i );
				throw;
			}
			checkerror( m_texture->UnlockRect( i ) );
		}
	}

	StaticTexture::StaticTexture(
		int format, utils::DataBuffer** levels, int width, int height )
		: m_format( format )
		, m_width( width )
		, m_height( height )
	{
		TABLE_ASSERT( formattable, format, "format" );
		std::atomic_thread_fence( std::memory_order_release );
		if( !levels || !levels[ 0 ] )
		{
			throw std::runtime_error( "bitmap data expected" );
		}
		for( int i = 0; i < 16; ++i )
		{
			if( !levels[ i ] )
			{
				m_levelcount = i - 1;
				break;
			}
			int lsize = texelsizetable[ format ] * width * height;
			if( levels[ i ]->m_length < lsize )
			{
				throw utils::StrException(
					"mip level %s's buffer is too small", i );
			}
			m_levels[ i ] = levels[ i ];
			width = width == 1 ? 1 : width >> 1;
			height = height == 1 ? 1 : height >> 1;
			if( width == 1 && height == 1 )
			{
				m_levelcount = i;
				break;
			}
		}
	}

	StaticTexture::~StaticTexture()
	{
	}

	bool StaticTexture::isformatsupported( int format ) noexcept
	{
		return format >= 0 && format < StaticTexture::Format_Invalid;
	}
}
