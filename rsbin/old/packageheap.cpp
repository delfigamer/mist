#include "packageheap.hpp"
#include "package.hpp"
#include "int.hpp"
#include <utils/functionaltask.hpp>
#include <utils/console.hpp>
#include <stdexcept>

#undef LOG
#define LOG( ... )

namespace rsbin {
	struct ChunkData {
		beu64 length;
		beu64 next;
	};

	struct HeapLoadTask: public utils::FunctionalTask {
		PackageHeap* m_heap;
		utils::Ref< Io > m_io;
		PackageHeader const* m_header;
		ChunkData* m_buffer;
		int m_ioresult;

		HeapLoadTask( int priority, PackageHeap* heap, Io* io, PackageHeader const* header ) :
			FunctionalTask( priority ) ,
			m_heap( heap ) ,
			m_io( io ) ,
			m_header( header ) ,
			m_buffer( 0 ) ,
			m_ioresult( -1 ) {
		}

		~HeapLoadTask() {
			delete[] m_buffer;
		}

		virtual void doaction() override;
	};

	void HeapLoadTask::doaction() {
		if( m_ioresult == -1 ) {
			m_buffer = new ChunkData[ m_header->heaplength ];
			m_subtask = m_io->startread( m_header->heapoffset, sizeof( ChunkData ) * m_header->heaplength,
				m_buffer, &m_ioresult, priority() + 1 );
			return;
		}
		if( m_ioresult != ( int )( sizeof( ChunkData ) * m_header->heaplength ) ) {
			throw std::runtime_error( "invalid input file" );
		}
		uint64_t offset = 0;
		for( int i = 0; i < ( int )m_header->heaplength; ++i ) {
			uint64_t length = m_buffer[ i ].length;
			if( length == 0 ) {
				throw std::runtime_error( "malformed package heap" );
			}
			m_heap->m_chunks[ offset ] = new PackageHeap::chunk_t( offset, length, 0 );
			offset += length;
		}
		m_heap->m_boundary = offset;
		PackageHeap::map_t::iterator it = m_heap->m_chunks.begin();
		for( int i = 0; i < ( int )m_header->heaplength; ++i ) {
			uint64_t next = m_buffer[ i ].next;
			if( next != ( uint64_t )( -1 ) ) {
				it->second->next = m_heap->m_chunks[ next ];
			}
			++it;
		}
		if( m_header->heapemptyspace == m_heap->m_boundary ) {
			m_heap->m_emptyspace = 0;
		} else {
			m_heap->m_emptyspace = m_heap->m_chunks[ m_header->heapemptyspace ];
		}
		m_heap->m_ready = true;
		setfinished( true );
	}

	PackageHeap::chunk_t::chunk_t( uint64_t _offset, uint64_t _length, chunk_t* _next ) :
		offset( _offset ) ,
		length( _length ) ,
		next( _next ) {
	}

	PackageHeap::chunk_t* PackageHeap::alloc( uint64_t length ) {
		chunk_t** prevptr = &m_emptyspace;
		chunk_t* next = m_emptyspace;
		while( next ) {
			if( next->length >= length ) {
				if( next->length == length ) {
					*prevptr = next->next;
				} else {
					chunk_t* newc = new chunk_t( next->offset + length, next->length - length, next->next );
					m_chunks[ newc->offset ] = newc;
					*prevptr = newc;
					next->length = length;
				}
				next->next = 0;
				return next;
			}
			prevptr = &next->next;
			next = next->next;
		}
		chunk_t* newc = new chunk_t( m_boundary, length, 0 );
		m_chunks[ newc->offset ] = newc;
		m_boundary += length;
		return newc;
	}

	PackageHeap::chunk_t* PackageHeap::normchain( chunk_t const* first ) {
		chunk_t* prev = const_cast< chunk_t* >( first );
		if( !prev ) {
			return 0;
		}
		chunk_t* next = prev->next;
		while( next ) {
			if( prev->offset + prev->length == next->offset ) {
				m_chunks.erase( next->offset );
				prev->length += next->length;
				prev->next = next->next;
				delete next;
				next = prev->next;
			} else {
				prev = next;
				next = next->next;
			}
		}
		return prev;
	}

	PackageHeap::PackageHeap() :
		m_emptyspace( 0 ) ,
		m_boundary( 0 ) {
		m_ready = false;
	}

	PackageHeap::PackageHeap( uint64_t firstlength ) :
		m_emptyspace( 0 ) ,
		m_boundary( firstlength ) {
		m_chunks[ 0 ] = new chunk_t( 0, firstlength, 0 );
		m_ready = true;
	}

	PackageHeap::~PackageHeap() {
		map_t::iterator it = m_chunks.begin();
		map_t::iterator eit = m_chunks.end();
		for( ; it != eit; ++it ) {
			delete it->second;
		}
	}

	PackageHeap::chunk_t const* PackageHeap::find( uint64_t offset ) {
		return m_chunks[ offset ];
	}

	uint64_t PackageHeap::extendchain( chunk_t const** first, uint64_t length ) {
		if( *first ) {
			chunk_t* chunk = const_cast< chunk_t* >( *first );
			while( chunk->next ) {
				chunk = chunk->next;
			}
			chunk->next = alloc( length );
			uint64_t result = chunk->next->offset;
			normchain( chunk );
			return result;
		} else {
			*first = alloc( length );
			return ( *first )->offset;
		}
	}

	void PackageHeap::freechain( chunk_t const* first ) {
		chunk_t* chunk = const_cast< chunk_t* >( first );
		chunk_t** esprevptr = &m_emptyspace;
		chunk_t* esnext = m_emptyspace;
		while( chunk ) {
			if( !esnext && m_emptyspace ) {
				esnext = m_emptyspace;
			}
			while( esnext && ( esnext->offset < chunk->offset ) ) {
				esprevptr = &esnext->next;
				esnext = esnext->next;
			}
			chunk_t* next = chunk->next;
			*esprevptr = chunk;
			chunk->next = esnext;
			chunk = next;
		}
		chunk = const_cast< chunk_t* >( normchain( m_emptyspace ) );
		if( chunk && ( chunk->offset + chunk->length == m_boundary ) ) {
			esprevptr = &m_emptyspace;
			esnext = m_emptyspace;
			while( esnext != chunk ) {
				esprevptr = &esnext->next;
				esnext = esnext->next;
			}
			m_boundary = chunk->offset;
			m_chunks.erase( chunk->offset );
			delete chunk;
			*esprevptr = 0;
		}
	}

	utils::TaskBase* PackageHeap::startload( Io* io, PackageHeader const* header, int taskpriority, utils::TaskQueueBase* queue ) {
		utils::Ref< HeapLoadTask > task = utils::Ref< HeapLoadTask >::create( taskpriority, this, io, header );
		queue->insert( task );
		return task;
	}

	void PackageHeap::write( Io* io, uint64_t offset ) {
		if( m_chunks.empty() ) {
			throw std::logic_error( "cannot write an empty PackageHeap" );
		}
		uint32_t heaplength = getlength();
		ChunkData* buffer = new ChunkData[ heaplength ];
		ChunkData* cd = buffer;
		map_t::iterator it = m_chunks.begin();
		map_t::iterator eit = m_chunks.end();
		for( ; it != eit; ++it ) {
			chunk_t* chunk = it->second;
			cd->length = chunk->length;
			cd->next = ( chunk->next ? chunk->next->offset : ( uint64_t )( -1 ) );
			++cd;
		}
		io->syncwrite( offset, heaplength * sizeof( ChunkData ), buffer );
		delete[] buffer;
	}

	uint32_t PackageHeap::getlength() {
		return m_chunks.size();
	}

	uint64_t PackageHeap::getemptyspace() {
		return m_emptyspace ? m_emptyspace->offset : m_boundary;
	}

	bool PackageHeap::isready() {
		return m_ready;
	}
}
