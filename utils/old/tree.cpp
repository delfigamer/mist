#include "tree.hpp"

namespace utils {
	TreeItem::ConstIterator::ConstIterator( const TreeItem* parent ) :
		m_listi( parent->m_children.iterator() ) {
	}
	
	bool TreeItem::ConstIterator::iterate( const TreeItem** next ) {
		const ListItem* nexti;
		if( m_listi.iterate( &nexti ) ) {
			*next = (const TreeItem*) nexti->data ();
			return true;
		} else {
			return false;
		}
	}
	
	TreeItem::Iterator::Iterator( TreeItem* parent ) :
		m_listi( parent->m_children.iterator() ) {
	}
	
	bool TreeItem::Iterator::iterate( TreeItem** next ) {
		ListItem* nexti;
		if( m_listi.iterate( &nexti ) ) {
			*next = (TreeItem*) nexti->data ();
			return true;
		} else {
			return false;
		}
	}
	
	PoolAllocator* TreeItem::m_allocator = requirepool< TreeItem >();
	
	TreeItem::TreeItem() :
		m_parent( 0 ) ,
		m_item( 0 ) ,
		m_children() ,
		m_data( 0 ) {
	}
	
	TreeItem::TreeItem( TreeItem* parent, void* data ) :
		m_children() {
		this->parent( parent );
		m_data = data;
	}
	
	TreeItem::~TreeItem() {
		parent( 0 );
	}
	
	const TreeItem* TreeItem::parent() const {
		return m_parent;
	}
	
	TreeItem* TreeItem::parent() {
		return m_parent;
	}
	
	TreeItem* TreeItem::parent( TreeItem* newparent ) {
		if( m_parent ) {
			delete m_item;
		}
		if( newparent ) {
			m_parent = newparent;
			m_item = new ListItem( &newparent->m_children, this );
		} else {
			m_parent = 0;
			m_item = 0;
		}
		return m_parent;
	}
	
	void* TreeItem::data() const {
		return m_data;
	}
	
	void* TreeItem::data( void* newdata ) {
		return m_data = newdata;
	}
	
	TreeItem::ConstIterator TreeItem::iterator() const {
		return ConstIterator( this );
	}
	
	TreeItem::Iterator TreeItem::iterator() {
		return Iterator( this );
	}
	
	void* TreeItem::operator new( size_t size ) {
		return m_allocator->alloc();
	}
	
	void TreeItem::operator delete( void* ptr ) {
		m_allocator->free( ptr );
	}
}
