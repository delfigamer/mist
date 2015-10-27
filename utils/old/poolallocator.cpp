#include "poolallocator.hpp"
#include <memory>

namespace utils {
	uint8_t* PoolAllocator::getlink( uint8_t const* ptr ) {
		return *( ( uint8_t* const* ) ptr );
	}

	void PoolAllocator::setlink( uint8_t* ptr, uint8_t const* link ) {
		*( ( uint8_t const** ) ptr ) = link;
	}

	uint8_t* PoolAllocator::newpage( uint8_t* nextptr, size_t itemsize, size_t itemcount ) {
		// uint8_t* page = new uint8_t[ ( itemcount * itemsize + ~PageSizeMask ) & PageSizeMask ];
		uint8_t* page = new uint8_t[ itemcount * itemsize ];
		uint8_t* ptr;
		uint8_t* link = page;
		for( size_t i = 1; i < itemcount; i++ ) {
			ptr = link;
			link = ptr + itemsize;
			setlink( ptr, link );
		}
		setlink( link, nextptr );
		return page;
	}

	PoolAllocator::PoolAllocator( size_t itemsize ) :
		PoolAllocator( itemsize, DefaultPageSize / itemsize ) {
	}

	PoolAllocator::PoolAllocator( size_t itemsize, size_t itemcount ) {
		if( itemsize < sizeof( uint8_t* ) ) {
			itemsize = sizeof( uint8_t* );
		}
		if( itemsize * itemcount > MaxPageSize ) {
			itemcount = MaxPageSize / itemcount;
		} else if( itemsize * itemcount < MinPageSize ) {
			itemcount = ( MinPageSize + itemsize - 1 ) / itemsize;
		}
		m_itemsize = itemsize;
		m_itemcount = itemcount;
		m_top = newpage( 0, m_itemsize, m_itemcount );
		m_pages.push_back( m_top );
	}

	PoolAllocator::~PoolAllocator() {
		pages_t::iterator it = m_pages.begin();
		pages_t::iterator eit = m_pages.end();
		for( ; it != eit; ++it ) {
			delete *it;
		}
	}

	void* PoolAllocator::alloc() {
		lock_t lock( m_mutex );
		if( m_top == 0 ) {
			m_top = newpage( 0, m_itemsize, m_itemcount );
			m_pages.push_back( m_top );
		}
		void* ptr = m_top;
		m_top = getlink( m_top );
		return ptr;
	}

	void PoolAllocator::free( void* ptr ) {
		lock_t lock( m_mutex );
		setlink( ( uint8_t* )ptr, m_top );
		m_top = ( uint8_t* )ptr;
	}
}
