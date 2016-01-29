#ifndef UTILS_COUNTERLOCK_HPP__
#define UTILS_COUNTERLOCK_HPP__ 1

#include <atomic>

namespace utils
{
	class CounterLock
	{
	private:
		std::atomic< int > m_first;
		std::atomic< int > m_last;

	public:
		CounterLock();
		~CounterLock();
		CounterLock( CounterLock const& ) = delete;
		CounterLock& operator=( CounterLock const& ) = delete;

		void lock() noexcept;
		void unlock() noexcept;
	};
}

#endif
