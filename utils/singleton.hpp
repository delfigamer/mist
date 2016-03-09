#ifndef UTILS_SINGLETON_HPP__
#define UTILS_SINGLETON_HPP__ 1

#include <utils/flaglock.hpp>
#include <common.hpp>
#include <mutex>

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

		T* get();

	public:
		Singleton();
		~Singleton();
		Singleton( Singleton const& other ) = delete;
		Singleton( Singleton&& other ) = delete;
		Singleton& operator=( Singleton const& other ) = delete;
		Singleton& operator=( Singleton&& other ) = delete;

		operator T*();
		T* operator->();
	};

	template< typename T >
	T* Singleton< T >::get()
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

	template< typename T >
	Singleton< T >::Singleton()
		: m_ptr( nullptr )
	{
	}

	template< typename T >
	Singleton< T >::~Singleton()
	{
		T* ptr = get();
		if( ptr )
		{
			ptr->~T();
			// do not deallocate the memory: Console depends on it
			// it's safe to do it, because singletons are destroyed right before the program is terminated
			// it's still nessessary to call the destructor to join separate threads
		}
	}

	template< typename T >
	Singleton< T >::operator T*()
	{
		return get();
	}

	template< typename T >
	T* Singleton< T >::operator->()
	{
		return get();
	}
}

#endif
