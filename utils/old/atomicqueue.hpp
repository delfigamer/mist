#ifndef UTILS_ATOMICQUEUE_HPP__
#define UTILS_ATOMICQUEUE_HPP__ 1

#include <mutex>
#include <atomic>
#include <cinttypes>

namespace utils {
	template< typename T >
	class AtomicQueue {
	public:
		struct Item {
			T data;
			Item* next;
			
			template< typename ...Ts >
			Item( Ts&& ...args ) : data( std::forward< Ts... >( args )... ) {}
		};
		
	private:
		typedef std::mutex mutex;
		typedef std::lock_guard< mutex > lock_guard;
		mutex m_mutex;
		Item* m_first;
		Item** m_plast;
		
	public:
		AtomicQueue();
		~AtomicQueue();
		AtomicQueue( AtomicQueue const& other ) = delete;
		AtomicQueue( AtomicQueue&& other ) = delete;
		AtomicQueue operator=( AtomicQueue const& other ) = delete;
		AtomicQueue operator=( AtomicQueue&& other ) = delete;
		
		void push( Item* item );
		void pop();
		Item* top();
	};
	
	template< typename T >
	AtomicQueue< T >::AtomicQueue() :
		m_plast( &m_first ) {
	}
	
	template< typename T >
	AtomicQueue< T >::~AtomicQueue() {
	}
	
	template< typename T >
	void AtomicQueue< T >::push( Item* item ) {
		lock_guard _lock_guard( mutex );
		item->next = 0;
		*m_plast = item;
		m_plast = &item->next;
	}
	
	template< typename T >
	void AtomicQueue< T >::pop() {
		lock_guard _lock_guard( mutex );
		m_first = m_first->next;
		if( m_first == 0 ) {
			m_plast = &m_first;
		}
	}
	
	template< typename T >
	typename AtomicQueue< T >::Item* AtomicQueue< T >::top() {
		lock_guard _lock_guard( mutex );
		return m_first;
	}
}

#endif
