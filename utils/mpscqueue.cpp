#include <utils/mpscqueue.hpp>
#include <memory>
#include <thread>

namespace utils
{
	MPSCQueueBase::Item* MPSCQueueBase::base_new()
	{
		// Item* item = ( Item* )m_allocator->alloc();
		Item* item = ( Item* )operator new( sizeof( Item ) + m_datasize );
		return new( item )Item;
	}

	void MPSCQueueBase::base_delete( MPSCQueueBase::Item* item )
	{
		item->~Item();
		// m_allocator->free( item );
		operator delete( item );
	}

	void MPSCQueueBase::base_push( MPSCQueueBase::Item* item )
	{
		Item* prev = m_last.exchange( item, std::memory_order_relaxed );
		prev->m_next.store( item, std::memory_order_release );
	}

	MPSCQueueBase::Item* MPSCQueueBase::base_peek()
	{
		Item* front = m_first->m_next.load( std::memory_order_acquire );
		return front;
	}

	void MPSCQueueBase::base_pop( Item* front )
	{
		base_delete( m_first );
		m_first = front;
	}

	MPSCQueueBase::MPSCQueueBase( int datasize )
		: m_datasize( datasize )
		// , m_allocator( new PoolAllocator( sizeof( Item ) + m_datasize, 1024 ) )
	{
		Item* dummy = base_new();
		m_first = dummy;
		m_last.store( dummy, std::memory_order_relaxed );
	}

	MPSCQueueBase::~MPSCQueueBase()
	{
		// delete m_allocator;
	}

	// bool MPSCQueueBase::isempty()
	// {
		// Item* prev = 0;
		// while( prev == 0 )
		// {
			// prev = m_first.load( std::memory_order_relaxed );
		// }
		// Item* front = prev->m_next.load( std::memory_order_relaxed );
		// return front == 0;
	// }
}
