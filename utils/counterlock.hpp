#ifndef UTILS_COUNTERLOCK_HPP__
#define UTILS_COUNTERLOCK_HPP__ 1

#include <atomic>

namespace utils {
	class CounterLock {
	private:
		std::atomic< int > m_first;
		std::atomic< int > m_last;
		
	public:
		CounterLock();
		~CounterLock();
		CounterLock( CounterLock const& other ) = delete;
		CounterLock( CounterLock&& other ) = delete;
		CounterLock& operator=( CounterLock const& other ) = delete;
		CounterLock& operator=( CounterLock&& other ) = delete;
		
		void lock() noexcept;
		void unlock() noexcept;
	};
}

#endif
