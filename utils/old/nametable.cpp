#include "nametable.hpp"

namespace utils {
	PoolAllocator* NameTable::m_blockallocator = requirepool< NameTable::Block >();
	
	NameTable::Block* NameTable::newblock() {
		Block* a = (Block*) m_blockallocator->alloc();
		int i;
		for( i = 0; i < 256; i++ ) {
			*a[ i ] = 0;
		}
		return a;
	}
	
	void** NameTable::find( unsigned int hash ) const {
		int b = hash >> 24;
		Block* ba = (Block*) *m_data[ b ];
		if(!ba) {
			*m_data[ b ] = ba = newblock();
		}
		b = (hash >> 16) & 0xff;
		Block* ca = (Block*) ba[ b ];
		if(!ca) {
			*ba[ b ] = ca = newblock();
		}
		b = (hash >> 8) & 0xff;
		Block* da = (Block*) ca[ b ];
		if(!da) {
			*ca[ b ] = da = newblock();
		}
		return *da + (hash & 0xff);
	}
	
	NameTable::NameTable() {
		m_data = (Block*) m_blockallocator->alloc();
	}
	
	NameTable::~NameTable() {
		int a;
		for( a = 0; a < 256; a++ ) {
			Block* ba = (Block*) m_data[ a ];
			if(ba) {
				int b;
				for( b = 0; b < 256; b++ ) {
					Block* ca = (Block*) ba[ b ];
					if(ca) {
						int c;
						for( c = 0; c < 256; c++ ) {
							void* da = ca[ c ];
							if(da) {
								m_blockallocator->free( da );
							}
						}
					}
				}
			}
		}
	}
	
	void const*& NameTable::operator[]( const Name& name ) const {
		return *(void const**)find( name.hash() );
	}
	
	void*& NameTable::operator[]( const Name& name ) {
		return *find( name.hash() );
	}
}
