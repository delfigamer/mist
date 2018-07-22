#pragma once

#include <common.hpp>
#include <atomic>
#include <utility>
#include <new>

// Multiple-producers-single-consumer queue
// push operation is atomic
// pop and peek operations are not thread-safe
template< typename T >
class MPSCQueue
{
public:
	typedef T value_type;

private:
	struct Item
	{
		char m_data[ sizeof( value_type ) ];
		std::atomic< Item* > m_next;
	};

public:
private:
	Item* m_first;
	std::atomic< Item* > m_last;

public:
	MPSCQueue();
	~MPSCQueue();
	MPSCQueue( const MPSCQueue& ) = delete;
	MPSCQueue& operator=( const MPSCQueue& ) = delete;

	template< typename... Ts >
	void push( Ts&&... args );
	bool peek( value_type* target = 0 );
	bool pop( value_type* target = 0 );
};

template< typename T >
MPSCQueue< T >::MPSCQueue()
{
	Item* dummy = new Item();
	dummy->m_next.store( nullptr, std::memory_order_relaxed );
	m_first = dummy;
	m_last.store( dummy, std::memory_order_relaxed );
}

template< typename T >
MPSCQueue< T >::~MPSCQueue()
{
	while( pop( nullptr ) )
	{
	}
	delete m_first;
}

template< typename T >
template< typename... Ts >
void MPSCQueue< T >::push( Ts&&... args )
{
	Item* item = new Item();
	try
	{
		new( item->m_data )value_type( std::forward< Ts >( args )... );
	}
	catch( ... )
	{
		delete item;
		throw;
	}
	Item* prev = m_last.exchange( item, std::memory_order_relaxed );
	prev->m_next.store( item, std::memory_order_release );
}

template< typename T >
bool MPSCQueue< T >::peek( typename MPSCQueue< T >::value_type* target )
{
	Item* item = m_first->m_next.load( std::memory_order_acquire );
	if( !item )
	{
		return false;
	}
	if( target != 0 )
	{
		*target = *( value_type* )item->m_data;
	}
	return true;
}

template< typename T >
bool MPSCQueue< T >::pop( typename MPSCQueue< T >::value_type* target )
{
	Item* item = m_first->m_next.load( std::memory_order_acquire );
	if( !item )
	{
		return false;
	}
	if( target != 0 )
	{
		*target = std::move( *( value_type* )item->m_data );
	}
	( ( value_type* )item->m_data )->~value_type();
	delete m_first;
	m_first = item;
	return true;
}
