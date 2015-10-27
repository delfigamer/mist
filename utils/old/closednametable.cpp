#include "closednametable.hpp"
#include <memory>

namespace utils {
	ClosedNameTable::Bucket::Bucket() :
		m_hash( 0 ) ,
		m_data( 0 ) {
	}
	
	ClosedNameTable::Bucket* ClosedNameTable::newblock( int capacity ) {
		Bucket* block = (Bucket*) heap_alloc( capacity * sizeof( Bucket ) );
		int i;
		for( i = 0; i < capacity; i++ ) {
			new( block + i ) Bucket();
		}
		return block;
	}
	
	ClosedNameTable::Bucket* ClosedNameTable::find( unsigned int hash ) const {
		int base = hash % m_capacity;
		int offset = m_capacity - 1;
		while( offset >= 0 ) {
			int i = base ^ offset;
			if( m_data[ i ].m_hash == 0 ) {
				m_data[ i ].m_hash = hash;
				return m_data + i;
			} else if( m_data[ i ].m_hash == hash ) {
				return m_data + i;
			}
			offset -= 1;
		}
		return 0;
	}
	
	ClosedNameTable::ClosedNameTable( int capacity ) :
		m_data( newblock( capacity ) ) ,
		m_capacity( capacity ) {
	}
	
	ClosedNameTable::~ClosedNameTable() {
		heap_free( m_data, m_capacity * sizeof( Bucket ) );
	}
	
	void const*& ClosedNameTable::operator[]( const Name& name ) const {
		return *(void const**)&find( name.hash() )->m_data;
	}
	
	void*& ClosedNameTable::operator[]( const Name& name ) {
		return find( name.hash() )->m_data;
	}
}
