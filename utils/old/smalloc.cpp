#include "smalloc.hpp"
#include <memory>

namespace utils {
	void* page_alloc( int size ) {
		return operator new( size );
	}
	
	void page_free( void* ptr, int size ) {
		operator delete( ptr );
	}
	
	void* heap_alloc( int size ) {
		return operator new( size );
	}
	
	void heap_free( void* ptr, int size ) {
		operator delete( ptr );
	}
	
	StackAllocator::StackAllocator() :
		StackAllocator( DefaultSize ) {
	}
	
	StackAllocator::StackAllocator( int size ) {
		m_base = page_alloc( size );
		m_pos = (Marker) 0;
		m_size = size;
	}
	
	StackAllocator::~StackAllocator() {
		page_free( m_base, m_size );
	}
	
	void* StackAllocator::alloc( int size ) {
		int newpos = (int) m_pos + size;
		if( newpos > m_size ) {
			return 0;
		}
		void* ptr = (void*) ( (char*) m_base + (int) m_pos );
		m_pos = (Marker) newpos;
		return ptr;
	}
	
	StackAllocator::Marker StackAllocator::marker() {
		return m_pos;
	}
	
	StackAllocator::Marker StackAllocator::marker( StackAllocator::Marker newmarker ) {
		return m_pos = newmarker;
	}
	
	void StackAllocator::clear() {
		m_pos = (Marker) 0;
	}
	
	void* PoolAllocator::getlink( void* ptr ) {
		return *( (void**) ptr );
	}
	
	void PoolAllocator::setlink( void* ptr, void* link ) {
		*( (void**) ptr ) = link;
	}
	
	void PoolAllocator::grow() {
		int i;
		void* base = page_alloc( m_itemcount * m_itemsize );
		void* ptr;
		void* link = (void*) ( (char*) base + m_itemsize );
		for( i = 1; i < (m_itemcount - 1); i++ ) {
			ptr = link;
			link = (void*) ( (char*) ptr + m_itemsize );
			setlink( ptr, link );
		}
		setlink( link, m_link );
		setlink( base, m_base );
		m_base = base;
		m_link = (void*) ( (char*) base + m_itemsize );
	}
	
	PoolAllocator::PoolAllocator( int itemsize ) :
		PoolAllocator( itemsize, DefaultCount ) {
	}
	
	PoolAllocator::PoolAllocator( int itemsize, int itemcount ) {
		m_base = 0;
		m_itemsize = itemsize;
		m_itemcount = itemcount;
		m_link = 0;
	}
	
	PoolAllocator::~PoolAllocator() {
		void* prev;
		void* next = m_base;
		while( next ) {
			prev = next;
			next = getlink( prev );
			page_free( prev, m_itemsize * m_itemcount );
		}
	}
	
	void* PoolAllocator::alloc() {
		if( m_link == 0 ) {
			grow();
		}
		void* ptr = m_link;
		m_link = getlink( ptr );
		return ptr;
	}
	
	void PoolAllocator::free( void* ptr ) {
		setlink( ptr, m_link );
		m_link = ptr;
	}
	
	StackAllocator GlobalStackAllocator( 0x10000 );
}
