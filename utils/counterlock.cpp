#include "counterlock.hpp"
#include <thread>

namespace utils {
	CounterLock::CounterLock() :
		m_first( 0 ) ,
		m_last( 0 ) {
	}

	CounterLock::~CounterLock() {
		while( m_first.load( std::memory_order_acquire ) != m_last.load( std::memory_order_acquire ) ) {
			unlock();
		}
	}

	void CounterLock::lock() noexcept {
		int index = m_last.fetch_add( 1, std::memory_order_acq_rel );
		while( m_first.load( std::memory_order_acquire ) != index ) {
			std::this_thread::yield();
		}
	}

	void CounterLock::unlock() noexcept {
		m_first.fetch_add( 1, std::memory_order_acq_rel );
	}
}
