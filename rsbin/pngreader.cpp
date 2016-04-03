#include <rsbin/pngreader.hpp>
#include <utils/strexception.hpp>
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <cstring>

namespace rsbin
{
	struct format_t
	{
		void( *setformat )(
			png_structp png, png_infop info,
			int bitdepth, int colortype );
		int pixelstride;
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

	format_t const formattable[] = {
		{ &setformat_rgba8, 4 },
	};

	void PngReader::error_handler( png_structp png, png_const_charp msg )
	{
		PngReader* reader = ( PngReader* )png_get_error_ptr( png );
		reader->m_error = msg;
		longjmp( reader->m_jmpbuf, 1 );
	}

	void PngReader::warning_handler( png_structp png, png_const_charp msg )
	{
		LOG( "png warning: %s", msg );
	}

	void PngReader::info_callback(
			png_structp png, png_infop info )
	{
		PngReader* reader =
			( PngReader* )png_get_progressive_ptr( png );
		int bitdepth;
		int colortype;
		png_get_IHDR( png, info,
			( png_uint_32* )&reader->m_width,
			( png_uint_32* )&reader->m_height,
			&bitdepth, &colortype, 0, 0, 0 );
		formattable[ reader->m_format ].setformat(
			png, info, bitdepth, colortype );
		png_set_interlace_handling( png );
		png_read_update_info( png, info );
		reader->m_stride =
			reader->m_width *
			formattable[ reader->m_format ].pixelstride;
		int size = reader->m_stride * reader->m_height;
		reader->m_data = utils::DataBuffer::create(
			size, size, 0 );
		memset( reader->m_data->m_data, 0, size );
	}

	void PngReader::row_callback(
			png_structp png, png_bytep row,
			png_uint_32 rowpos, int pass )
	{
		if( !row )
		{
			return;
		}
		PngReader* reader =
			( PngReader* )png_get_progressive_ptr( png );
		png_progressive_combine_row(
			png,
			reader->m_data->m_data +
				reader->m_stride * ( reader->m_height - rowpos - 1 ),
			row );
	}

	void PngReader::end_callback(
			png_structp png, png_infop info )
	{
		PngReader* reader =
			( PngReader* )png_get_progressive_ptr( png );
		reader->m_finished = true;
	}

	PngReader::PngReader( bitmapformat format )
		: m_format( int( format ) )
		, m_finished( false )
		, m_width( 0 )
		, m_height( 0 )
		, m_data( nullptr )
		, m_png( 0 )
		, m_info( 0 )
	{
		if( m_format < 0 || m_format >= int( bitmapformat::invalid ) )
		{
			throw std::runtime_error( "invalid bitmap format" );
		}
		m_png = png_create_read_struct( PNG_LIBPNG_VER_STRING, this,
			&PngReader::error_handler, &PngReader::warning_handler );
		if( !m_png )
		{
			throw std::runtime_error( "out of memory" );
		}
		m_info = png_create_info_struct( m_png );
		if( !m_info )
		{
			png_destroy_read_struct( &m_png, 0, 0 );
			throw std::runtime_error( "out of memory" );
		}
		png_set_progressive_read_fn(
			m_png, this, &PngReader::info_callback,
			&PngReader::row_callback, &PngReader::end_callback );
	}

	PngReader::~PngReader()
	{
		if( m_png )
		{
			png_destroy_read_struct(
				&m_png, m_info ? &m_info : 0, 0 );
		}
	}

	void PngReader::feed( int length, void const* buffer )
	{
		if( setjmp( m_jmpbuf ) )
		{
			throw utils::StrException( m_error );
		}
		png_process_data(
			m_png, m_info, png_bytep( buffer ), length );
	}
}
