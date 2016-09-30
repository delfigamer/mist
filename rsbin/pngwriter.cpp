#include <rsbin/pngwriter.hpp>
#include <common/strexception.hpp>
#include <utils/console.hpp>
#include <stdexcept>

namespace rsbin
{
	struct format_t
	{
		ptrdiff_t pixelstride;
		int bitdepth;
		int colortype;
		double invgamma;
	};

	format_t const formattable[] =
	{
		{ 4, 8, PNG_COLOR_TYPE_RGB_ALPHA, 1 / 2.2 },
	};

	enum
	{
		stage_writeheader = 0,
		stage_writerow = 1,
		stage_writeend = 2,
		stage_finished = 10,
	};

	void PngWriter::error_handler(
		png_structp png, png_const_charp msg )
	{
		PngWriter* writer = ( PngWriter* )png_get_error_ptr( png );
		writer->m_error = msg;
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
		writer->m_buffer.push( int( length ), data );
	}

	void PngWriter::flush_callback( png_structp png )
	{
	}

	void PngWriter::writeheader()
	{
		m_png = png_create_write_struct(
			PNG_LIBPNG_VER_STRING, this,
			&PngWriter::error_handler,
			&PngWriter::warning_handler );
		m_info = png_create_info_struct( m_png );
		png_set_write_fn(
			m_png, this,
			&PngWriter::write_callback,
			&PngWriter::flush_callback );
		format_t const* format = &formattable[ m_format ];
		png_set_IHDR(
			m_png, m_info, m_width, m_height, format->bitdepth,
			format->colortype, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
		png_set_gAMA( m_png, m_info, format->invgamma );
		png_write_info( m_png, m_info );
		m_stage = stage_writerow;
	}

	void PngWriter::writerow()
	{
		png_write_row(
			m_png,
			m_data->m_data +
				m_stride * ( m_height - m_rowpos - 1 ) );
		m_rowpos += 1;
		if( m_rowpos == m_height )
		{
			m_stage = stage_writeend;
		}
	}

	void PngWriter::writeend()
	{
		png_write_end( m_png, 0 );
		m_stage = stage_finished;
	}

	bool PngWriter::feedbuffer()
	{
		if( setjmp( m_jmpbuf ) )
		{
			throw StrException( m_error );
		}
		switch( m_stage )
		{
		case stage_writeheader:
			writeheader();
			break;
		case stage_writerow:
			writerow();
			break;
		case stage_writeend:
			writeend();
			break;
		default:
			return false;
		}
		return true;
	}

	PngWriter::PngWriter(
		bitmapformat format, uint32_t width, uint32_t height,
		DataBuffer* data )
		: m_format( int( format ) )
		, m_stage( stage_writeheader )
		, m_width( width )
		, m_height( height )
		, m_rowpos( 0 )
		, m_data( data )
		, m_png( 0 )
		, m_info( 0 )
	{
		if( m_format < 0 || m_format >= int( bitmapformat::invalid ) )
		{
			throw std::runtime_error( "invalid bitmap format" );
		}
		m_stride = width * formattable[ m_format ].pixelstride;
	}

	PngWriter::~PngWriter()
	{
		if( m_png )
		{
			png_destroy_write_struct( &m_png, m_info ? &m_info : 0 );
		}
	}

	size_t PngWriter::grab( size_t length, void* buffer )
	{
		size_t result = 0;
		while( length > 0 )
		{
			size_t advance = m_buffer.pop( length, buffer );
			length -= advance;
			buffer = ( uint8_t* )buffer + advance;
			result += advance;
			if( !feedbuffer() )
			{
				break;
			}
		}
		size_t advance = m_buffer.pop( length, buffer );
		length -= advance;
		buffer = ( uint8_t* )buffer + advance;
		result += advance;
		return result;
	}

	bool PngWriter::isfinished()
	{
		return m_stage == stage_finished && m_buffer.isempty();
	}
}
