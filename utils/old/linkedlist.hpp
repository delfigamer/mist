#ifndef UTILS_LINKEDLIST_HPP__
#define UTILS_LINKEDLIST_HPP__ 1

#include <mutex>
#include <atomic>
#include <type_traits>
#include <exception>

namespace utils {
	template< typename T >
	class LinkedList {
	public:
		typedef typename std::remove_reference< T >::type ItemType;
		
	private:
		class Container {
		public:
			Container* m_prev;
			Container* m_next;
			
		public:
			Container() = delete;
			Container( Container* prev, Container* next );
			Container( Container const& ) = delete;
			Container( Container&& ) = delete;
			~Container() noexcept;
			Container& operator=( Container const& ) = delete;
			Container& operator=( Container&& ) = delete;
		};
		
		class RootContainer: public Container {
		public:
			RootContainer();
			RootContainer( RootContainer const& ) = delete;
			RootContainer( RootContainer&& ) = delete;
			~RootContainer() noexcept;
			RootContainer& operator=( RootContainer const& ) = delete;
			RootContainer& operator=( RootContainer&& ) = delete;
		};
		
		class DataContainer: public Container {
		private:
			std::atomic< ItemType > m_data;
			
		public:
			DataContainer( Container* prev, Container* next, ItemType const& data );
			DataContainer( Container* prev, Container* next, ItemType&& data );
			DataContainer( DataContainer const& ) = delete;
			DataContainer( DataContainer&& ) = delete;
			~DataContainer() noexcept;
			DataContainer& operator=( DataContainer const& ) = delete;
			DataContainer& operator=( DataContainer&& ) = delete;
			DataContainer& operator=( ItemType const& data );
			DataContainer& operator=( ItemType&& data );
			operator ItemType();
		};
		
		std::recursive_mutex m_mutex;
		RootContainer m_rc;
		
	public:
		class Iterator {
		private:
			LinkedList* m_list;
			Container* m_pos;
			
		public:
			Iterator() = delete;
			Iterator( LinkedList* list, Container* pos );
			Iterator( Iterator const& ) noexcept = default;
			Iterator( Iterator&& ) noexcept = default;
			~Iterator() noexcept = default;
			Iterator& operator=( Iterator const& ) noexcept = default;
			Iterator& operator=( Iterator&& ) noexcept = default;
			Iterator& operator++();
			Iterator operator++( int );
			Iterator& operator--();
			Iterator operator--( int );
			bool operator==( Iterator const& other ) const;
			bool operator!=( Iterator const& other ) const;
			DataContainer& operator*();
		};
		
		LinkedList();
		LinkedList( LinkedList const& ) = delete;
		LinkedList( LinkedList&& ) = delete;
		~LinkedList() noexcept;
		LinkedList& operator=( LinkedList const& ) = delete;
		LinkedList& operator=( LinkedList&& ) = delete;
		
		Iterator append( ItemType const& data );
		Iterator append( ItemType&& data );
		void remove( Iterator& pos );
		void clear();
		Iterator begin();
		Iterator end();
	};
	
	template< typename T >
	LinkedList< T >::Container::Container( LinkedList< T >::Container* prev, LinkedList< T >::Container* next ) :
		m_prev( prev ) ,
		m_next( next ) {
	}
	
	template< typename T >
	LinkedList< T >::Container::~Container() noexcept {
		m_prev->m_next = m_next;
		m_next->m_prev = m_prev;
	}
	
	template< typename T >
	LinkedList< T >::RootContainer::RootContainer() :
		Container( this, this ) {
	}
	
	template< typename T >
	LinkedList< T >::RootContainer::~RootContainer() noexcept {
	}
	
	template< typename T >
	LinkedList< T >::DataContainer::DataContainer( LinkedList< T >::Container* prev, LinkedList< T >::Container* next, LinkedList< T >::ItemType const& data ) :
		Container( prev, next ) ,
		m_data( data ) {
		prev->m_next = this;
		next->m_prev = this;
	}
	
	template< typename T >
	LinkedList< T >::DataContainer::DataContainer( LinkedList< T >::Container* prev, LinkedList< T >::Container* next, LinkedList< T >::ItemType&& data ) :
		Container( prev, next ) ,
		m_data( std::move( data ) ) {
		prev->m_next = this;
		next->m_prev = this;
	}
	
	template< typename T >
	LinkedList< T >::DataContainer::~DataContainer() noexcept {
	}
	
	template< typename T >
	typename LinkedList< T >::DataContainer& LinkedList< T >::DataContainer::operator=( LinkedList< T >::ItemType const& data ) {
		m_data = data;
	}
	
	template< typename T >
	typename LinkedList< T >::DataContainer& LinkedList< T >::DataContainer::operator=( LinkedList< T >::ItemType&& data ) {
		m_data = std::move( data );
	}
	
	template< typename T >
	LinkedList< T >::DataContainer::operator LinkedList< T >::ItemType() {
		return m_data;
	}
	
	template< typename T >
	LinkedList< T >::Iterator::Iterator( LinkedList< T >* list, LinkedList< T >::Container* pos ) {
		m_list = list;
		m_pos = pos;
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator& LinkedList< T >::Iterator::operator++() {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_list->m_mutex );
		m_pos = m_pos->m_next;
		return *this;
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator LinkedList< T >::Iterator::operator++( int ) {
		Iterator copy = *this;
		operator++();
		return copy;
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator& LinkedList< T >::Iterator::operator--() {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_list->m_mutex );
		m_pos = m_pos->m_prev;
		return *this;
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator LinkedList< T >::Iterator::operator--( int ) {
		Iterator copy = *this;
		operator--();
		return copy;
	}
	
	template< typename T >
	bool LinkedList< T >::Iterator::operator==( Iterator const& other ) const {
		return m_pos == other.m_pos;
	}
	
	template< typename T >
	bool LinkedList< T >::Iterator::operator!=( Iterator const& other ) const {
		return m_pos != other.m_pos;
	}
	
	template< typename T >
	typename LinkedList< T >::DataContainer& LinkedList< T >::Iterator::operator*() {
		if( m_pos == &(m_list->m_rc) ) {
			throw std::runtime_error( "dereferencing an invalid iterator" );
		}
		return *(DataContainer*)m_pos;
	}
	
	template< typename T >
	LinkedList< T >::LinkedList() {
	}
	
	template< typename T >
	LinkedList< T >::~LinkedList() noexcept {
		clear();
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator LinkedList< T >::append( LinkedList< T >::ItemType const& data ) {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_mutex );
		DataContainer* dc = new DataContainer( m_rc.m_prev, &m_rc, data );
		return Iterator( this, dc );
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator LinkedList< T >::append( LinkedList< T >::ItemType&& data ) {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_mutex );
		DataContainer* dc = new DataContainer( m_rc.m_prev, &m_rc, std::move( data ) );
		return Iterator( this, dc );
	}
	
	template< typename T >
	void LinkedList< T >::remove( LinkedList< T >::Iterator& pos ) {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_mutex );
		delete &(*pos);
		pos = end();
	}
	
	template< typename T >
	void LinkedList< T >::clear() {
		while( m_rc.m_next != &m_rc ) {
			delete (DataContainer*)m_rc.m_next;
		}
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator LinkedList< T >::begin() {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_mutex );
		return Iterator( this, m_rc.m_next );
	}
	
	template< typename T >
	typename LinkedList< T >::Iterator LinkedList< T >::end() {
		std::lock_guard< std::recursive_mutex > _lock_guard( m_mutex );
		return Iterator( this, &m_rc );
	}
}

#endif