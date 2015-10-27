#include "pngreadtask.hpp"
#include <utils/console.hpp>
#include <utils/strexception.hpp>
#include <utils/cbase.hpp>
#include <stdexcept>
#include <cstring>

namespace rsbin {
	static int const stage_readsig = 0;
	static int const stage_readheader = 1;
	static int const stage_processchunk = 2;
	static int const stage_finished = 10;

	static int ChunkSize = 0x10000;	// 64K

	void PngReadTask::error_handler( png_structp png, png_const_charp msg ) {
		PngReadTask* task = ( PngReadTask* )png_get_error_ptr( png );
		task->m_error = msg;
		longjmp( task->m_jmpbuf, 1 );
	}

	void PngReadTask::warning_handler( png_structp png, png_const_charp msg ) {
		LOG( "png warning: %s", msg );
	}

	void PngReadTask::info_callback( png_structp png, png_infop info ) {
		PngReadTask* task = ( PngReadTask* )png_get_progressive_ptr( png );
		int bitdepth;
		int colortype;
		double gamma;
		png_get_IHDR( png, info,
			( png_uint_32* )&task->m_width, ( png_uint_32* )&task->m_height,
			&bitdepth, &colortype, 0, 0, 0 );
		if( png_get_gAMA( png, info, &gamma ) ) {
			png_set_gamma( png, 2.2, gamma );
		} else {
			png_set_gamma( png, 2.2, 0.45455 );
		}
		if( colortype == PNG_COLOR_TYPE_PALETTE ) {
			png_set_palette_to_rgb( png );
		}
		if( colortype == PNG_COLOR_TYPE_GRAY && bitdepth < 8 ) {
			png_set_expand_gray_1_2_4_to_8( png );
		}
		if( png_get_valid( png, info, PNG_INFO_tRNS ) ) {
			png_set_tRNS_to_alpha( png );
		} else {
			int channels = png_get_channels( png, info );
			if( channels == 1 || channels == 3 ) {
				png_set_add_alpha( png, 255, PNG_FILLER_AFTER );
			}
		}
		if( colortype == PNG_COLOR_TYPE_GRAY || colortype == PNG_COLOR_TYPE_GRAY_ALPHA ) {
			png_set_gray_to_rgb( png );
		}
		if( bitdepth == 16 ) {
			png_set_scale_16( png );
		}
		png_set_interlace_handling( png );
		png_read_update_info( png, info );
		task->m_stride = task->m_width * 4;
		int size = task->m_stride * task->m_height;
		task->m_data = utils::StringInstance::create( size, size, 0 );
		memset( task->m_data->m_chars, 0, size );
	}

	void PngReadTask::row_callback( png_structp png, png_bytep row,
		png_uint_32 rowpos, int pass ) {
		if( !row ) {
			return;
		}
		PngReadTask* task = ( PngReadTask* )png_get_progressive_ptr( png );
		png_progressive_combine_row(
			png, task->m_data->m_chars + task->m_stride * ( task->m_height - rowpos - 1 ), row );
	}

	void PngReadTask::end_callback( png_structp png, png_infop info ) {
		PngReadTask* task = ( PngReadTask* )png_get_progressive_ptr( png );
		task->m_stage = stage_finished;
		task->setfinished( true );
	}

	void PngReadTask::doreadsig() {
		m_subtask = m_source->starttransfer( ChunkSize, m_buffer, &m_ioresult, priority() + 1 );
		m_stage = stage_readheader;
	}

	void PngReadTask::doreadheader() {
		if( m_ioresult < 8 || png_sig_cmp( m_buffer, 0, 8 ) != 0 ) {
			throw std::runtime_error( "invalid png stream" );
		}
		m_png = png_create_read_struct( PNG_LIBPNG_VER_STRING, this,
			&PngReadTask::error_handler, &PngReadTask::warning_handler );
		if( !m_png ) {
			throw std::runtime_error( "out of memory" );
		}
		m_info = png_create_info_struct( m_png );
		if( !m_info ) {
			throw std::runtime_error( "out of memory" );
		}
		png_set_progressive_read_fn( m_png, this, &PngReadTask::info_callback,
			&PngReadTask::row_callback, &PngReadTask::end_callback );
		png_process_data( m_png, m_info, m_buffer, m_ioresult );
		if( m_stage != stage_finished ) {
			m_subtask = m_source->starttransfer( ChunkSize, m_buffer,
				&m_ioresult, priority() + 1 );
			m_stage = stage_processchunk;
		}
	}

	void PngReadTask::doprocesschunk() {
		if( m_ioresult == 0 ) {
			throw std::runtime_error( "invalid png stream" );
		}
		png_process_data( m_png, m_info, m_buffer, m_ioresult );
		if( m_stage != stage_finished ) {
			m_subtask = m_source->starttransfer( ChunkSize, m_buffer,
				&m_ioresult, priority() + 1 );
		}
	}

	bool PngReadTask::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	PngReadTask::PngReadTask( int taskpriority, utils::Stream* source ) :
		utils::FunctionalTask( taskpriority ) ,
		m_source( source ) ,
		m_stage( stage_readsig ) ,
		m_width( 0 ) ,
		m_height( 0 ) ,
		m_data( nullptr ) ,
		m_buffer( 0 ) ,
		m_png( 0 ) ,
		m_info( 0 ) {
		m_buffer = ( uint8_t* )operator new( ChunkSize );
	}

	PngReadTask::~PngReadTask() {
		operator delete( m_buffer );
		if( m_png ) {
			png_destroy_read_struct( &m_png, m_info ? &m_info : 0, 0 );
		}
	}

	void PngReadTask::doaction() {
		if( setjmp( m_jmpbuf ) ) {
			throw utils::StrException( m_error );
		}
		switch( m_stage ) {
		case stage_readsig:
			doreadsig();
			break;
		case stage_readheader:
			doreadheader();
			break;
		case stage_processchunk:
			doprocesschunk();
			break;
		default:
			setfinished( true );
		}
	}

	int PngReadTask::getwidth() const {
		return m_width;
	}

	int PngReadTask::getheight() const {
		return m_height;
	}

	utils::Ref< utils::StringInstance > const& PngReadTask::getdata() const {
		return m_data;
	}

	extern "C" {
		PngReadTask* rsbin_pngreadtask_new( int taskpriority, utils::Stream* source ) noexcept {
			CBASE_PROTECT(
				return new PngReadTask( taskpriority, source );
			)
		}

		int rsbin_pngreadtask_getwidth( PngReadTask* task ) noexcept {
			CBASE_PROTECT(
				return task->getwidth();
			)
		}

		int rsbin_pngreadtask_getheight( PngReadTask* task ) noexcept {
			CBASE_PROTECT(
				return task->getheight();
			)
		}

		utils::StringInstance* rsbin_pngreadtask_getdata( PngReadTask* task ) noexcept {
			CBASE_PROTECT(
				return task->getdata();
			)
		}
	}
}
