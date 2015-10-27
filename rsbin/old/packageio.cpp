#include "packageio.hpp"
#include "fsthread.hpp"
#include <utils/functionaltask.hpp>
#include <utils/taskqueue.hpp>
#include <utils/ref.hpp>
#include <utils/console.hpp>
#include <stdexcept>

#undef LOG
#define LOG( ... )

namespace rsbin {
	int const DefaultChunkSize = 0x100;

	enum IoAction: bool {
		IoActionRead = false,
		IoActionWrite = true
	};

	struct PackageIoIoTask: public utils::FunctionalTask {
		utils::Ref< PackageIo > m_pio;
		uint64_t m_offset;
		int m_length;
		void* m_buffer;
		int m_result;
		int* m_resultptr;
		IoAction m_ioaction;
		PackageHeap::chunk_t const* m_chunk;
		int m_ioresult;

		PackageIoIoTask( int priority, PackageIo* pio, uint64_t offset,
			int length, void* buffer, int* resultptr, IoAction ioaction );

		~PackageIoIoTask();
		virtual void doaction() override;
	};

	PackageIoIoTask::PackageIoIoTask( int priority, PackageIo* pio, uint64_t offset,
		int length, void* buffer, int* resultptr, IoAction ioaction ) :
		FunctionalTask( priority ) ,
		m_pio( pio ) ,
		m_offset( offset ) ,
		m_length( length ) ,
		m_buffer( buffer ) ,
		m_result( 0 ) ,
		m_resultptr( resultptr ) ,
		m_ioaction( ioaction ) ,
		m_chunk( pio->m_space ) ,
		m_ioresult( -1 ) {
	}

	PackageIoIoTask::~PackageIoIoTask() {
	}

	void PackageIoIoTask::doaction() {
		if( m_ioresult == 0 ) {
			if( m_resultptr ) {
				*m_resultptr = m_result;
			}
			setfinished( true );
			return;
		} else if( m_ioresult != -1 ) {
			m_offset += m_ioresult;
			m_result += m_ioresult;
			m_buffer = ( char* )m_buffer + m_ioresult;
			m_length -= m_ioresult;
		}
		if( m_length == 0 ) {
			if( m_resultptr ) {
				*m_resultptr = m_result;
			}
			setfinished( true );
			return;
		}
		while( m_chunk->length <= m_offset ) {
			if( !m_chunk->next ) {
				if( m_resultptr ) {
					*m_resultptr = m_result;
				}
				setfinished( true );
				return;
			}
			m_offset -= m_chunk->length;
			m_chunk = m_chunk->next;
		}
		int length = m_chunk->length - m_offset;
		if( length > m_length ) {
			length = m_length;
		}
		switch( m_ioaction ) {
		case IoActionRead:
			m_subtask = m_pio->m_base->startread( m_chunk->offset + m_offset, length, m_buffer, &m_ioresult, priority() + 1 );
			break;

		case IoActionWrite:
			m_subtask = m_pio->m_base->startwrite( m_chunk->offset + m_offset, length, m_buffer, &m_ioresult, priority() + 1 );
			break;
		}
	}

	bool PackageIo::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	PackageIo::PackageIo( Io* base, PackageHeap* heap, PackageHeap::chunk_t const* space ) :
		m_base( base ) ,
		m_heap( heap ) ,
		m_space( space ) {
		if( !space ) {
			if( !heap ) {
				throw std::logic_error( "PackageIo with no space and heap created" );
			}
			heap->extendchain( &m_space, DefaultChunkSize );
			m_maxsize = DefaultChunkSize;
		} else {
			m_maxsize = 0;
			PackageHeap::chunk_t const* ch = space;
			while( ch ) {
				m_maxsize += ch->length;
				ch = ch->next;
			}
		}
	}

	PackageIo::~PackageIo() {
	}

	utils::TaskBase* PackageIo::dostartread( uint64_t offset, int length, void* buffer, int* result, int taskpriority ) {
		PackageIoIoTask* task = new PackageIoIoTask (
			taskpriority, this, offset, length, buffer, result, IoActionRead );
		utils::DefaultQueue->insert( task );
		return task;
	}

	utils::TaskBase* PackageIo::dostartwrite( uint64_t offset, int length, void const* buffer, int* result, int taskpriority ) {
		extend( offset + length );
		PackageIoIoTask* task = new PackageIoIoTask (
			taskpriority, this, offset, length, const_cast< void* >( buffer ), result, IoActionWrite );
		utils::DefaultQueue->insert( task );
		return task;
	}

	uint64_t PackageIo::maxsize() {
		return m_maxsize;
	}

	void logchain( PackageHeap::chunk_t const* chunk ) {
		while( chunk ) {
			LOG( "\t\t( 0x%.8x ) %#18llx .. %#18llx", uint32_t( chunk ), chunk->offset, chunk->offset + chunk->length );
			chunk = chunk->next;
		}
	}

	bool PackageIo::extend( uint64_t minsize ) {
//		LOG( "\t( ( rsbin::PackageIo* )0x%.8x )->extend( 0x%llx )",
//			( unsigned )this, minsize );
		if( minsize <= m_maxsize ) {
//			LOG( "\t( ( rsbin::PackageIo* )0x%.8x )->extend( 0x%llx )\tminsize <= m_maxsize",
//				( unsigned )this, minsize );
			return false;
		}
		if( !m_heap ) {
			throw std::runtime_error( "extend() called for a PackageIo without a valid heap" );
		}
		while( m_maxsize < minsize ) {
//			LOG( "\t( ( rsbin::PackageIo* )0x%.8x )->extend( 0x%llx )\tm_maxsize < minsize",
//				( unsigned )this, minsize );
			m_heap->extendchain( &m_space, m_maxsize );
// 			logchain( m_space );
			m_maxsize <<= 1;
		}
		return true;
	}

	PackageHeap::chunk_t const* PackageIo::getspace() {
		return m_space;
	}
}
