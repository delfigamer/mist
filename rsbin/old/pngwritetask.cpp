#include "pngwritetask.hpp"
#include <utils/console.hpp>
#include <utils/strexception.hpp>
#include <utils/cbase.hpp>
#include <stdexcept>
#include <cstring>

namespace rsbin {
	enum {
		stage_writeheader = 0,
		stage_writechunk = 1,
		stage_writeend = 2,
		stage_finished = 10,
	};
	static int const ChunkSize = 0x10000;

	void PngWriteTask::error_handler( png_structp png, png_const_charp msg ) {
		PngWriteTask* task = ( PngWriteTask* )png_get_error_ptr( png );
		task->m_error = msg;
		longjmp( task->m_jmpbuf, 1 );
	}

	void PngWriteTask::warning_handler( png_structp png, png_const_charp msg ) {
		LOG( "png warning: %s", msg );
	}

	void PngWriteTask::write_callback( png_structp png, png_bytep data, png_size_t length ) {
		PngWriteTask* task = ( PngWriteTask* )png_get_io_ptr( png );
		void* m_buffer = task->m_buffer + task->m_bufferfill;
		if( int( length ) > ( ChunkSize - task->m_bufferfill ) ) {
			png_error( png, "attempt to write a chunk too large" );
		}
		memcpy( m_buffer, data, length );
// 		LOG( "%4i %.2x%.2x%.2x%.2x %.2x%.2x%.2x%.2x %.2x%.2x%.2x%.2x %.2x%.2x%.2x%.2x", length,
// 			( ( uint8_t* )m_buffer )[ 0 ], ( ( uint8_t* )m_buffer )[ 1 ], ( ( uint8_t* )m_buffer )[ 2 ], ( ( uint8_t* )m_buffer )[ 3 ],
// 			( ( uint8_t* )m_buffer )[ 4 ], ( ( uint8_t* )m_buffer )[ 5 ], ( ( uint8_t* )m_buffer )[ 6 ], ( ( uint8_t* )m_buffer )[ 7 ],
// 			( ( uint8_t* )m_buffer )[ 8 ], ( ( uint8_t* )m_buffer )[ 9 ], ( ( uint8_t* )m_buffer )[ 10 ], ( ( uint8_t* )m_buffer )[ 11 ],
// 			( ( uint8_t* )m_buffer )[ 12 ], ( ( uint8_t* )m_buffer )[ 13 ], ( ( uint8_t* )m_buffer )[ 14 ], ( ( uint8_t* )m_buffer )[ 15 ] );
		task->m_bufferfill += length;
// 		LOG( "%i", task->m_bufferfill );
	}

	void PngWriteTask::flush_callback( png_structp png ) {
// 		LOG( "-" );
	}

	void PngWriteTask::dowriteheader() {
// 		LOG( "-" );
		m_png = png_create_write_struct( PNG_LIBPNG_VER_STRING, this,
			&PngWriteTask::error_handler, &PngWriteTask::warning_handler );
		m_info = png_create_info_struct( m_png );
		png_set_write_fn( m_png, this,
			&PngWriteTask::write_callback, &PngWriteTask::flush_callback );
		png_set_IHDR( m_png, m_info, m_width, m_height, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
		png_set_gAMA( m_png, m_info, 1 / 2.2 );
		png_write_info( m_png, m_info );
		m_stage = stage_writechunk;
		flushbuffer();
	}

	void PngWriteTask::dowritechunk() {
// 		LOG( "-" );
		do {
			if( m_rowpos == m_height ) {
				break;
			}
			png_write_row( m_png,
				m_data->m_chars + m_stride * ( m_height - m_rowpos - 1 ) );
			m_rowpos += 1;
		} while( !flushbuffer() );
		if( m_rowpos == m_height ) {
			m_stage = stage_writeend;
		}
	}

	void PngWriteTask::dowriteend() {
// 		LOG( "-" );
		png_write_end( m_png, 0 );
		m_stage = stage_finished;
		flushbuffer();
	}

	bool PngWriteTask::flushbuffer() {
		if( m_bufferfill > 0 ) {
// 		LOG( "%4i %.2x%.2x%.2x%.2x %.2x%.2x%.2x%.2x %.2x%.2x%.2x%.2x %.2x%.2x%.2x%.2x", m_bufferfill,
// 			( ( uint8_t* )m_buffer )[ 0 ], ( ( uint8_t* )m_buffer )[ 1 ], ( ( uint8_t* )m_buffer )[ 2 ], ( ( uint8_t* )m_buffer )[ 3 ],
// 			( ( uint8_t* )m_buffer )[ 4 ], ( ( uint8_t* )m_buffer )[ 5 ], ( ( uint8_t* )m_buffer )[ 6 ], ( ( uint8_t* )m_buffer )[ 7 ],
// 			( ( uint8_t* )m_buffer )[ 8 ], ( ( uint8_t* )m_buffer )[ 9 ], ( ( uint8_t* )m_buffer )[ 10 ], ( ( uint8_t* )m_buffer )[ 11 ],
// 			( ( uint8_t* )m_buffer )[ 12 ], ( ( uint8_t* )m_buffer )[ 13 ], ( ( uint8_t* )m_buffer )[ 14 ], ( ( uint8_t* )m_buffer )[ 15 ] );
			m_subtask = m_target->starttransfer( m_bufferfill, m_buffer,
				&m_ioresult, priority() + 1 );
			return true;
		} else {
			return false;
		}
	}

	bool PngWriteTask::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	PngWriteTask::PngWriteTask( int taskpriority, utils::Stream* target, int width, int height, utils::StringInstance* data ) :
		utils::FunctionalTask( taskpriority ) ,
		m_target( target ) ,
		m_stage( stage_writeheader ) ,
		m_width( width ) ,
		m_height( height ) ,
		m_stride( width << 2 ) ,
		m_rowpos( 0 ) ,
		m_data( data ) ,
		m_buffer( 0 ) ,
		m_ioresult( 0 ) ,
		m_png( 0 ) ,
		m_info( 0 ) {
// 		LOG( "-" );
		m_buffer = ( uint8_t* )operator new( ChunkSize );
// 		LOG( "-" );
		m_bufferfill = 0;
// 		LOG( "-" );
	}

	PngWriteTask::~PngWriteTask() {
		operator delete( m_buffer );
		if( m_png ) {
			png_destroy_write_struct( &m_png, m_info ? &m_info : 0 );
		}
	}

	void PngWriteTask::doaction() {
		if( setjmp( m_jmpbuf ) ) {
			throw utils::StrException( m_error );
		}
		if( m_ioresult > 0 ) {
			if( m_ioresult != m_bufferfill ) {
				throw std::runtime_error( "failure writing to io" );
			}
			m_ioresult = 0;
			m_bufferfill = 0;
		}
		switch( m_stage ) {
		case stage_writeheader:
			dowriteheader();
			break;
		case stage_writechunk:
			dowritechunk();
			break;
		case stage_writeend:
			dowriteend();
			break;
		default:
			setfinished( true );
		}
	}

	extern "C" {
		PngWriteTask* rsbin_pngwritetask_new( int taskpriority, utils::Stream* target,
			int width, int height, utils::StringInstance* data ) noexcept {
		CBASE_PROTECT(
			return new PngWriteTask( taskpriority, target, width, height, data );
		)
		}
	}
}
