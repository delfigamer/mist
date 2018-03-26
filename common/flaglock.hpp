#pragma once

#include <common.hpp>
#include <atomic>

class FlagLock
{
private:
	std::atomic_flag m_flag;

public:
	FlagLock()
	{
		m_flag.clear( std::memory_order_release );
	}

	~FlagLock()
	{
	}

	FlagLock( FlagLock const& ) = delete;
	FlagLock& operator=( FlagLock const& ) = delete;

	bool trylock() noexcept
	{
		return m_flag.test_and_set( std::memory_order_acquire );
	}

	void lock() noexcept
	{
		while( m_flag.test_and_set( std::memory_order_acquire ) )
		{
		}
	}

	void unlock() noexcept
	{
		m_flag.clear( std::memory_order_release );
	}
};
