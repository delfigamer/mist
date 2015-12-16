#ifndef UTILS_MPSCQUEUE_HPP__
#define UTILS_MPSCQUEUE_HPP__ 1

// #include "poolallocator.hpp"
#include <atomic>
#include <utility>

namespace utils {
	// Multiple-producers-single-consumer queue
	// push operation is atomic
	// pop and peek operations are not thread-safe
	class MPSCQueueBase
	{
	protected:
		struct Item
		{
			std::atomic< Item* > m_next;
			uint8_t m_data[];

			inline Item() : m_next( nullptr ) {}
		};

	private:
		int const m_datasize;
		Item* m_first;
		std::atomic< Item* > m_last;
		// PoolAllocator* m_allocator;

	protected:
		Item* base_new();
		void base_delete( Item* item );
		void base_push( Item* item );
		Item* base_peek();
		void base_pop( Item* front );

	public:
		MPSCQueueBase() = delete;
		MPSCQueueBase( int datasize );
		~MPSCQueueBase();
		MPSCQueueBase( const MPSCQueueBase& other ) = delete;
		MPSCQueueBase( MPSCQueueBase&& other ) = delete;
		MPSCQueueBase& operator=(
			const MPSCQueueBase& other ) = delete;
		MPSCQueueBase& operator=( MPSCQueueBase&& other ) = delete;

		// bool isempty();
	};

	template< typename T >
	class MPSCQueue: public MPSCQueueBase
	{
	public:
		typedef T value_type;

	public:
		MPSCQueue() : MPSCQueueBase( sizeof( value_type ) ) {}
		~MPSCQueue()
		{
			while( pop() )
			{
			}
		}
		MPSCQueue( const MPSCQueue& other ) = delete;
		MPSCQueue( MPSCQueue&& other ) = delete;
		MPSCQueue& operator=( const MPSCQueue& other ) = delete;
		MPSCQueue& operator=( MPSCQueue&& other ) = delete;

		template< typename... Ts >
		void push( Ts&&... args )
		{
			Item* item = base_new();
			try
			{
				new( item->m_data )value_type(
					std::forward< Ts >( args )... );
			}
			catch( ... )
			{
				base_delete( item );
				throw;
			}
			base_push( item );
		}

		bool peek( value_type* target = 0 )
		{
			Item* item = base_peek();
			if( item == 0 )
			{
				return false;
			}
			value_type* data = ( value_type* )item->m_data;
			if( target != 0 )
			{
				*target = *data;
			}
			return true;
		}

		bool pop( value_type* target = 0 )
		{
			Item* item = base_peek();
			if( item == 0 )
			{
				return false;
			}
			try
			{
				value_type* data = ( value_type* )item->m_data;
				if( target != 0 )
				{
					*target = std::move( *data );
				}
				data->~value_type();
			}
			catch( ... )
			{
				base_pop( item );
				throw;
			}
			base_pop( item );
			return true;
		}
	};
}

#endif
