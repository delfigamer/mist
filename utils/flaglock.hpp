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
		FlagLock( FlagLock const& other ) = delete;
		FlagLock( FlagLock&& other ) = delete;
		FlagLock& operator=( FlagLock const& other ) = delete;
		FlagLock& operator=( FlagLock&& other ) = delete;

		void lock() noexcept;
		void unlock() noexcept;
	};
}

#endif
