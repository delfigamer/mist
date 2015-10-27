#include "list.hpp"

namespace utils {
	ListItem::ConstIterator::ConstIterator( const ListItem* root ) {
		m_root = root;
		m_next = root->m_next;
	}
	
	bool ListItem::ConstIterator::iterate( const ListItem** next ) {
		if( m_next == m_root ) {
			return false;
		} else {
			*next = m_next;
			m_next = m_next->m_next;
			return true;
		}
	}
	
	ListItem::Iterator::Iterator( ListItem* root ) {
		m_root = root;
		m_next = root->m_next;
	}
	
	bool ListItem::Iterator::iterate( ListItem** next ) {
		if( m_next == m_root ) {
			return false;
		} else {
			*next = m_next;
			m_next = m_next->m_next;
			return true;
		}
	}
	
	PoolAllocator* ListItem::m_allocator = requirepool< ListItem >();
	
	ListItem::ListItem() :
		m_next( this ) ,
		m_prev( this ) ,
		m_data( 0 ) {
	}
	
	ListItem::ListItem( ListItem* next, void* data ) :
		m_next( next ) ,
		m_prev( next->m_prev ) ,
		m_data( data ) {
		m_prev->m_next = this;
		m_next->m_prev = this;
	}
	
	ListItem::~ListItem() {
		remove();
	}
	
	void ListItem::remove() {
		m_prev->m_next = m_next;
		m_next->m_prev = m_prev;
		m_next = this;
		m_prev = this;
	}
	
	const ListItem* ListItem::next() const {
		return m_next;
	}
	
	const ListItem* ListItem::prev() const {
		return m_prev;
	}
	
	ListItem* ListItem::next() {
		return m_next;
	}
	
	ListItem* ListItem::prev() {
		return m_prev;
	}
	
	void* ListItem::data() const {
		return m_data;
	}
	
	void* ListItem::data( void* newdata ) {
		return m_data = newdata;
	}
	
	ListItem::ConstIterator ListItem::iterator() const {
		return ConstIterator( this );
	}
	
	ListItem::Iterator ListItem::iterator() {
		return Iterator( this );
	}
	
	void* ListItem::operator new( size_t size ) {
		return m_allocator->alloc();
	}
	
	void ListItem::operator delete( void* ptr ) {
		m_allocator->free( ptr );
	}
}
