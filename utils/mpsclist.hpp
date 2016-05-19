#pragma once

#include <common.hpp>
#include <atomic>
#include <utility>
#include <new>
#include <stdexcept>

namespace utils
{
	// Multiple-producers-single-consumer list
	// push operation is atomic
	// iteration is not thread-safe
	template< typename T >
	class MPSCList
	{
	public:
		typedef T value_type;

	private:
		struct Item
		{
			std::atomic< Item* > m_next;
			std::atomic< bool > m_dead;
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
				, m_dead( false )
			{
			}

			~Item()
			{
			}
		};

	public:
		class Iterator
		{
		private:
			Item* m_current;

		public:
			Iterator( Item* current );
			~Iterator() = default;
			Iterator( Iterator const& ) = default;
			Iterator& operator=( Iterator const& ) = default;

			Iterator& operator++();
			value_type& operator*();
			bool operator==( Iterator const& other );
			bool operator!=( Iterator const& other );
			void kill();
		};

	private:
		Item* m_first;
		std::atomic< Item* > m_last;

		void appenditem( Item* item );

	public:
		MPSCList();
		~MPSCList();
		MPSCList( const MPSCList& ) = delete;
		MPSCList& operator=( const MPSCList& ) = delete;

		template< typename... Ts >
		Iterator append( Ts&&... args );
		Iterator begin();
		Iterator end();
	};

	template< typename T >
	MPSCList< T >::Iterator::Iterator( Item* current )
		: m_current( current )
	{
	}

	template< typename T >
	typename MPSCList< T >::Iterator& MPSCList< T >::Iterator::operator++()
	{
		Item* replace = 0;
		Item* next = m_current->m_next.load( std::memory_order_acquire );
		while( next && next->m_dead.load( std::memory_order_relaxed ) )
		{
			Item* repl = next->m_next.load( std::memory_order_acquire );
			if( !repl )
			{
				next = 0;
				break;
			}
#if defined( _MSC_VER )
			( ( value_type* )next->m_data )->~value_type();
#else
			next->m_data.~value_type();
#endif
			delete next;
			replace = repl;
			next = repl;
		}
		if( replace )
		{
			m_current->m_next.store( replace, std::memory_order_relaxed );
		}
		m_current = next;
		return *this;
	}

	template< typename T >
	typename MPSCList< T >::value_type& MPSCList< T >::Iterator::operator*()
	{
		if( m_current )
		{
#if defined( _MSC_VER )
			return *( value_type* )m_current->m_data;
#else
			return m_current->m_data;
#endif
		}
		else
		{
			throw std::runtime_error( "dereferencing an invalid iterator" );
		}
	}

	template< typename T >
	bool MPSCList< T >::Iterator::operator==(
		typename MPSCList< T >::Iterator const& other )
	{
		return m_current == other.m_current;
	}

	template< typename T >
	bool MPSCList< T >::Iterator::operator!=(
		typename MPSCList< T >::Iterator const& other )
	{
		return !( *this == other );
	}

	template< typename T >
	void MPSCList< T >::Iterator::kill()
	{
		if( m_current )
		{
			m_current->m_dead.store( true, std::memory_order_relaxed );
			m_current = 0;
		}
	}

	template< typename T >
	void MPSCList< T >::appenditem( typename MPSCList< T >::Item* item )
	{
		Item* prev = m_last.exchange( item, std::memory_order_relaxed );
		prev->m_next.store( item, std::memory_order_release );
	}

	template< typename T >
	MPSCList< T >::MPSCList()
	{
		Item* dummy = new Item();
		m_first = dummy;
		m_last.store( dummy, std::memory_order_relaxed );
	}

	template< typename T >
	MPSCList< T >::~MPSCList()
	{
		Item* item = m_first->m_next.load( std::memory_order_relaxed );
		while( item )
		{
			Item* next = item->m_next.load( std::memory_order_relaxed );
#if defined( _MSC_VER )
			( ( value_type* )item->m_data )->~value_type();
#else
			item->m_data.~value_type();
#endif
			delete item;
			item = next;
		}
		delete m_first;
	}

	template< typename T >
	template< typename... Ts >
	typename MPSCList< T >::Iterator MPSCList< T >::append( Ts&&... args )
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
		return Iterator( item );
	}

	template< typename T >
	typename MPSCList< T >::Iterator MPSCList< T >::begin()
	{
		return ++( Iterator( m_first ) );
	}

	template< typename T >
	typename MPSCList< T >::Iterator MPSCList< T >::end()
	{
		return Iterator( 0 );
	}
}
