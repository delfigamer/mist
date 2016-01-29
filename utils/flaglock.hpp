#ifndef UTILS_FLAGLOCK_HPP__
#define UTILS_FLAGLOCK_HPP__ 1

#include <atomic>

namespace utils
{
	class FlagLock
	{
	private:
		std::atomic_flag m_flag;

	public:
		FlagLock();
		~FlagLock();
		FlagLock( FlagLock const& ) = delete;
		FlagLock& operator=( FlagLock const& ) = delete;

		void lock() noexcept;
		void unlock() noexcept;
	};
}

#endif
