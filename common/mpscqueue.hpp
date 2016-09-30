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
		std::atomic< Item* > m_next;
#if defined( _MSC_VER )
		char m_data[ sizeof( value_type ) ];
#else
		union {
			value_type m_data;
			int m_dummy;
		};
#endif

		Item()
			: m_next( nullptr )
		{
		}

		~Item()
		{
		}
	};

public:
private:
	Item* m_first;
	std::atomic< Item* > m_last;

	void appenditem( Item* item );

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
void MPSCQueue< T >::appenditem( typename MPSCQueue< T >::Item* item )
{
	Item* prev = m_last.exchange( item, std::memory_order_relaxed );
	prev->m_next.store( item, std::memory_order_release );
}

template< typename T >
MPSCQueue< T >::MPSCQueue()
{
	Item* dummy = new Item();
	m_first = dummy;
	m_last.store( dummy, std::memory_order_relaxed );
}

template< typename T >
MPSCQueue< T >::~MPSCQueue()
{
	//
	delete m_first;
}

template< typename T >
template< typename... Ts >
void MPSCQueue< T >::push( Ts&&... args )
{
	Item* item = new Item();
	try
	{
#if defined( _MSC_VER )
		new( item->m_data )value_type(
			std::forward< Ts >( args )... );
#else
		new( &item->m_data )value_type(
			std::forward< Ts >( args )... );
#endif
	}
	catch( ... )
	{
		delete item;
		throw;
	}
	appenditem( item );
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
#if defined( _MSC_VER )
		*target = *( value_type* )item->m_data;
#else
		*target = item->m_data;
#endif
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
#if defined( _MSC_VER )
		*target = std::move( *( value_type* )item->m_data );
#else
		*target = std::move( item->m_data );
#endif
	}
#if defined( _MSC_VER )
	( ( value_type* )item->m_data )->~value_type();
#else
	item->m_data.~value_type();
#endif
	delete m_first;
	m_first = item;
	return true;
}
