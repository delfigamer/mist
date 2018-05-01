#include <rsbin/pngwriter.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <string>

namespace rsbin
{
	struct format_t
	{
		uint32_t pixelstride;
		int bitdepth;
		int colortype;
		double invgamma;
	};

	format_t const formattable[] =
	{
		{ 4, 8, PNG_COLOR_TYPE_RGB_ALPHA, 0.45455 },
		{ 8, 16, PNG_COLOR_TYPE_RGB_ALPHA, 0.45455 },
	};

	void PngWriter::error_handler(
		png_structp png, png_const_charp msg )
	{
		PngWriter* writer = ( PngWriter* )png_get_error_ptr( png );
		writer->m_error = std::make_exception_ptr(
			std::runtime_error( std::string( msg ) ) );
		longjmp( writer->m_jmpbuf, 1 );
	}

	void PngWriter::warning_handler(
		png_structp png, png_const_charp msg )
	{
		LOG( "png warning: %s", msg );
	}

	void PngWriter::write_callback(
		png_structp png, png_bytep data, png_size_t length )
	{
		PngWriter* writer = ( PngWriter* )png_get_io_ptr( png );
		try
		{
			while( length > 0 )
			{
				StorageMap map = writer->advance( length );
				if( length < map.length )
				{
					map.length = length;
				}
				memcpy( map.ptr, data, map.length );
				length -= map.length;
				data += map.length;
			}
		}
		catch( ... )
		{
			writer->m_error = std::current_exception();
			longjmp( writer->m_jmpbuf, 1 );
		}
	}

	void PngWriter::flush_callback( png_structp png )
	{
	}

	void PngWriter::writedata( png_structp png, png_infop info )
	{
		if( setjmp( m_jmpbuf ) )
		{
			return;
		}
		format_t const& format = formattable[ m_format ];
		png_set_IHDR(
			png, info, m_width, m_height, format.bitdepth,
			format.colortype, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
		png_set_gAMA( png, info, format.invgamma );
		png_write_info( png, info );
		if( format.bitdepth > 8 )
		{
			png_set_swap( png );
		}
		abortpoint();
		for( uint32_t row = 0; row < m_height; ++row )
		{
			png_write_row(
				png,
				m_pixels->m_data + m_stride * ( m_height - row - 1 ) );
			abortpoint();
		}
		png_write_end( png, nullptr );
	}

	void PngWriter::process()
	{
		png_structp png = png_create_write_struct(
			PNG_LIBPNG_VER_STRING, this,
			&error_handler, &warning_handler );
		if( png )
		{
			png_infop info = png_create_info_struct( png );
			if( info )
			{
				png_set_write_fn(
					png, this,
					&write_callback, &flush_callback );
				writedata( png, info );
			}
			else
			{
				m_error = std::make_exception_ptr(
					std::runtime_error( "out of memory" ) );
			}
			png_destroy_write_struct(
				&png,
				info ? &info : nullptr );
		}
		else
		{
			m_error = std::make_exception_ptr(
				std::runtime_error( "out of memory" ) );
		}
	}

	PngWriter::PngWriter(
		Stream* stream, bitmapformat format,
		uint32_t width, uint32_t height, DataBuffer* pixels )
		: FormatTask( stream )
		, m_pixels( pixels )
		, m_width( width )
		, m_height( height )
		, m_format( int( format ) )
	{
		if( m_format < 0 || m_format >= int( bitmapformat::invalid ) )
		{
			throw std::runtime_error( "invalid bitmap format" );
		}
		externalassert( pixels );
		m_stride = width * formattable[ m_format ].pixelstride;
		externalassert( pixels->m_length >= m_stride * height );
		start();
	}

	PngWriter::~PngWriter()
	{
	}
}
