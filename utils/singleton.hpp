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
		std::atomic< int > m_refcount;

	public:
		Singleton();
		~Singleton();
		Singleton( Singleton const& other ) = delete;
		Singleton( Singleton&& other ) = delete;
		Singleton& operator=( Singleton const& other ) = delete;
		Singleton& operator=( Singleton&& other ) = delete;

		T* get();
		void release();
	};

	template< typename T >
	class SingletonRef
	{
	private:
		Singleton< T >* m_source;
		T* m_ptr;

	public:
		SingletonRef() = delete;
		SingletonRef( Singleton< T >& source );
		~SingletonRef();
		SingletonRef( SingletonRef const& other ) = delete;
		SingletonRef( SingletonRef&& other ) = delete;
		SingletonRef& operator=( SingletonRef const& other ) = delete;
		SingletonRef& operator=( SingletonRef&& other ) = delete;

		T& operator*() const;
		T* operator->() const;
		operator T*() const;
	};

	template< typename T >
	Singleton< T >::Singleton()
		: m_ptr( nullptr )
		, m_refcount( 0 )
	{
	}

	template< typename T >
	Singleton< T >::~Singleton()
	{
	}

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
		m_refcount.fetch_add( 1, std::memory_order_relaxed );
		return ptr;
	}

	template< typename T >
	void Singleton< T >::release()
	{
		if( m_refcount.fetch_sub( 1, std::memory_order_relaxed ) == 1 )
		{
			lock_t lock( m_mutex );
			T* ptr = m_ptr.exchange( 0, std::memory_order_acq_rel );
			if( ptr )
			{
				delete ptr;
			}
		}
	}

	template< typename T >
	SingletonRef< T >::SingletonRef( Singleton< T >& source )
		: m_source( &source )
		, m_ptr( source.get() )
	{
	}

	template< typename T >
	SingletonRef< T >::~SingletonRef()
	{
		m_source->release();
	}

	template< typename T >
	T& SingletonRef< T >::operator*() const
	{
		return *m_ptr;
	}

	template< typename T >
	T* SingletonRef< T >::operator->() const
	{
		return m_ptr;
	}

	template< typename T >
	SingletonRef< T >::operator T*() const
	{
		return m_ptr;
	}
}

#endif
