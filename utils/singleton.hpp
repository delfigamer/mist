#ifndef UTILS_SINGLETON_HPP__
#define UTILS_SINGLETON_HPP__ 1

#include "flaglock.hpp"
#include <mutex>
#include <atomic>

namespace utils
{
	template< typename T >
	class Singleton
	{
	private:
		typedef FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		std::atomic< T* > m_ptr;

	public:
		Singleton();
		~Singleton();
		Singleton( Singleton const& other ) = delete;
		Singleton( Singleton&& other ) = delete;
		Singleton& operator=( Singleton const& other ) = delete;
		Singleton& operator=( Singleton&& other ) = delete;

		T* operator()();
	};

	template< typename T >
	Singleton< T >::Singleton()
		: m_ptr( nullptr )
	{
	}

	template< typename T >
	Singleton< T >::~Singleton()
	{
		lock_t lock( m_mutex );
		T* ptr = m_ptr.load( std::memory_order_acquire );
		if( ptr )
		{
			delete ptr;
		}
	}

	template< typename T >
	T* Singleton< T >::operator()()
	{
		T* ptr = m_ptr.load( std::memory_order_acquire );
		if( ptr == 0 )
		{
			lock_t lock( m_mutex );
			ptr = m_ptr.load( std::memory_order_acquire );
			if( ptr == 0 )
			{
				ptr = new T();
				m_ptr.store( ptr, std::memory_order_release );
			}
		}
		return ptr;
	}
}

#endif
