#include <rsbin/pngreader.hpp>
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
		ptrdiff_t pixelstride;
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
		reader->m_error = std::make_exception_ptr(
			std::runtime_error( std::string( msg ) ) );
		longjmp( reader->m_jmpbuf, 1 );
	}

	void PngReader::warning_handler( png_structp png, png_const_charp msg )
	{
		LOG( "png warning: %s", msg );
	}

	void PngReader::info_callback( png_structp png, png_infop info )
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
			reader->m_width * formattable[ reader->m_format ].pixelstride;
		size_t size = reader->m_stride * reader->m_height;
		reader->m_pixels = DataBuffer::create( size );
		memset( reader->m_pixels->m_data, 0, size );
	}

	void PngReader::row_callback(
		png_structp png, png_bytep row, png_uint_32 rowpos, int pass )
	{
		if( !row )
		{
			return;
		}
		PngReader* reader =
			( PngReader* )png_get_progressive_ptr( png );
		png_progressive_combine_row(
			png,
			reader->m_pixels->m_data +
				reader->m_stride * ( reader->m_height - rowpos - 1 ),
			row );
	}

	void PngReader::end_callback( png_structp png, png_infop info )
	{
		PngReader* reader =
			( PngReader* )png_get_progressive_ptr( png );
		reader->m_streamend = true;
	}

	void PngReader::readdata( png_structp png, png_infop info )
	{
		if( setjmp( m_jmpbuf ) )
		{
			return;
		}
		try
		{
			while( true )
			{
				Ref< MapTask > task = m_stream->startadvance( uint32_t( -1 ) );
				if( !task )
				{
					break;
				}
				while( !task->poll() )
				{
					if( m_aborted.load( std::memory_order_relaxed ) )
					{
						m_error = std::make_exception_ptr(
							std::runtime_error( "aborted" ) );
						return;
					}
					std::this_thread::yield();
				}
				StorageMap map = task->getmap();
				if( !map.ptr || map.length == 0 )
				{
					break;
				}
				png_process_data(
					png, info, png_bytep( map.ptr ), map.length );
				if( m_streamend )
				{
					return;
				}
				if( m_aborted.load( std::memory_order_relaxed ) )
				{
					m_error = std::make_exception_ptr(
						std::runtime_error( "aborted" ) );
					return;
				}
			}
			m_error = std::make_exception_ptr(
				std::runtime_error( "input stream terminated" ) );
		}
		catch( ... )
		{
			m_error = std::current_exception();
		}
	}

	void PngReader::threadfunc()
	{
		png_structp png = png_create_read_struct(
			PNG_LIBPNG_VER_STRING, this,
			&PngReader::error_handler, &PngReader::warning_handler );
		if( png )
		{
			png_infop info = png_create_info_struct( png );
			if( info )
			{
				png_set_progressive_read_fn(
					png, this,
					&info_callback, &row_callback, &end_callback );
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
		m_finished.store( true, std::memory_order_release );
	}

	PngReader::PngReader( bitmapformat format, Stream* stream )
		: m_width( 0 )
		, m_height( 0 )
		, m_streamend( false )
		, m_aborted( false )
		, m_finished( false )
	{
		if( m_format < 0 || m_format >= int( bitmapformat::invalid ) )
		{
			throw std::runtime_error( "invalid bitmap format" );
		}
		externalassert( stream );
		m_stream = stream;
		m_thread = std::thread( threadfunc, this );
	}

	PngReader::~PngReader()
	{
		m_aborted.store( true, std::memory_order_relaxed );
		m_thread.join();
	}

	bool PngReader::poll()
	{
		if( m_finished.load( std::memory_order_acquire ) )
		{
			if( m_error )
			{
				std::rethrow_exception( m_error );
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	Ref< DataBuffer > PngReader::getpixels()
	{
		if( m_finished.load( std::memory_order_acquire ) )
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
		if( m_finished.load( std::memory_order_acquire ) )
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
		if( m_finished.load( std::memory_order_acquire ) )
		{
			return m_height;
		}
		else
		{
			return 0;
		}
	}
}
