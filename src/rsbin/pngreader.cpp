#include <rsbin/pngreader.hpp>
#include <utils/console.hpp>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace rsbin
{
	struct format_t
	{
		void( *setformat )(
			png_structp png, png_infop info,
			int bitdepth, int colortype );
		uint32_t pixelstride;
	};

	void setformat_rgba8(
		png_structp png, png_infop info,
		int bitdepth, int colortype )
	{
		double gamma;
		if( png_get_gAMA( png, info, &gamma ) )
		{
			png_set_gamma( png, 2.2, gamma );
		}
		else
		{
			png_set_gamma( png, 2.2, 0.45455 );
		}
		if( colortype == PNG_COLOR_TYPE_PALETTE )
		{
			png_set_palette_to_rgb( png );
		}
		if( colortype == PNG_COLOR_TYPE_GRAY && bitdepth < 8 )
		{
			png_set_expand_gray_1_2_4_to_8( png );
		}
		if( png_get_valid( png, info, PNG_INFO_tRNS ) )
		{
			png_set_tRNS_to_alpha( png );
		}
		else
		{
			int channels = png_get_channels( png, info );
			if( channels == 1 || channels == 3 )
			{
				png_set_add_alpha( png, 255, PNG_FILLER_AFTER );
			}
		}
		if( colortype == PNG_COLOR_TYPE_GRAY ||
			colortype == PNG_COLOR_TYPE_GRAY_ALPHA )
		{
			png_set_gray_to_rgb( png );
		}
		if( bitdepth == 16 )
		{
			png_set_scale_16( png );
		}
	}

	void setformat_rgba16(
		png_structp png, png_infop info,
		int bitdepth, int colortype )
	{
		double gamma;
		if( png_get_gAMA( png, info, &gamma ) )
		{
			png_set_gamma( png, 2.2, gamma );
		}
		else
		{
			png_set_gamma( png, 2.2, 0.45455 );
		}
		if( colortype == PNG_COLOR_TYPE_PALETTE )
		{
			png_set_palette_to_rgb( png );
		}
		if( bitdepth < 16 )
		{
			png_set_expand_16( png );
		}
		if( png_get_valid( png, info, PNG_INFO_tRNS ) )
		{
			png_set_tRNS_to_alpha( png );
		}
		else
		{
			int channels = png_get_channels( png, info );
			if( channels == 1 || channels == 3 )
			{
				png_set_add_alpha( png, 0xffff, PNG_FILLER_AFTER );
			}
		}
		if( colortype == PNG_COLOR_TYPE_GRAY ||
			colortype == PNG_COLOR_TYPE_GRAY_ALPHA )
		{
			png_set_gray_to_rgb( png );
		}
		png_set_swap( png );
	}

	format_t const formattable[] = {
		{ &setformat_rgba8, 4 },
		{ &setformat_rgba16, 8 },
	};

	void PngReader::error_handler( png_structp png, png_const_charp msg )
	{
		PngReader* reader = ( PngReader* )png_get_error_ptr( png );
		reader->m_error = std::make_exception_ptr(
			std::runtime_error( std::string( msg ) ) );
		longjmp( reader->m_jmpbuf, 1 );
	}

	void PngReader::warning_handler( png_structp png, png_const_charp msg )
	{
		LOG( "png warning: %s", msg );
	}

	void PngReader::read_callback(
		png_structp png, png_bytep data, png_size_t length )
	{
		PngReader* reader = ( PngReader* )png_get_io_ptr( png );
		try
		{
			while( length > 0 )
			{
				StorageMap map = reader->advance( length );
				if( length < map.length )
				{
					map.length = length;
				}
				memcpy( data, map.ptr, map.length );
				length -= map.length;
				data += map.length;
			}
		}
		catch( ... )
		{
			reader->m_error = std::current_exception();
			longjmp( reader->m_jmpbuf, 1 );
		}
	}

	void PngReader::readdata( png_structp png, png_infop info )
	{
		if( setjmp( m_jmpbuf ) )
		{
			return;
		}
		png_read_info( png, info );
		abortpoint();
		int bitdepth;
		int colortype;
		png_get_IHDR( png, info,
			( png_uint_32* )&m_width,
			( png_uint_32* )&m_height,
			&bitdepth, &colortype, 0, 0, 0 );
		formattable[ m_format ].setformat(
			png, info, bitdepth, colortype );
		png_set_interlace_handling( png );
		png_read_update_info( png, info );
		m_stride = m_width * formattable[ m_format ].pixelstride;
		assert( png_get_rowbytes( png, info ) == m_stride );
		size_t size = m_stride * m_height;
		m_pixels = DataBuffer::create( size );
		memset( m_pixels->m_data, 0, size );
		std::vector< uint8_t* > rowpointers( m_height );
		for( uint32_t y = 0; y < m_height; ++y )
		{
			rowpointers[ y ] = m_pixels->m_data + m_stride * ( m_height - y - 1 );
		}
		png_read_image( png, ( png_bytep* )rowpointers.data() );
		abortpoint();
		png_read_end( png, nullptr );
	}

	void PngReader::process()
	{
		png_structp png = png_create_read_struct(
			PNG_LIBPNG_VER_STRING, this,
			&error_handler, &warning_handler );
		if( png )
		{
			png_infop info = png_create_info_struct( png );
			if( info )
			{
				png_set_read_fn( png, this,
					&read_callback );
				readdata( png, info );
			}
			else
			{
				m_error = std::make_exception_ptr(
					std::runtime_error( "out of memory" ) );
			}
			png_destroy_read_struct(
				&png,
				info ? &info : nullptr,
				nullptr );
		}
		else
		{
			m_error = std::make_exception_ptr(
				std::runtime_error( "out of memory" ) );
		}
	}

	PngReader::PngReader( Stream* stream, bitmapformat format )
		: FormatTask( stream )
		, m_width( 0 )
		, m_height( 0 )
		, m_format( int( format ) )
		, m_streamend( false )
	{
		if( m_format < 0 || m_format >= int( bitmapformat::invalid ) )
		{
			throw std::runtime_error( "invalid bitmap format" );
		}
		start();
	}

	PngReader::~PngReader()
	{
	}

	Ref< DataBuffer > PngReader::getpixels()
	{
		if( isfinished() )
		{
			return m_pixels;
		}
		else
		{
			return nullptr;
		}
	}

	uint32_t PngReader::getwidth()
	{
		if( isfinished() )
		{
			return m_width;
		}
		else
		{
			return 0;
		}
	}

	uint32_t PngReader::getheight()
	{
		if( isfinished() )
		{
			return m_height;
		}
		else
		{
			return 0;
		}
	}
}
