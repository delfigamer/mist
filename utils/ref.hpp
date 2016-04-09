#ifndef UTILS_REF_HPP__
#define UTILS_REF_HPP__ 1

#include <utils/refobject.hpp>
#include <common.hpp>
#include <atomic>
#include <stdexcept>
#include <exception>
#include <utility>
#include <cstddef>

namespace utils
{
	template< typename T >
	class Ref
	{
	public:
		template< typename U > friend class Ref;

	private:
		std::atomic< T* > m_ref;

	public:
		Ref() NOEXCEPT;
		Ref( std::nullptr_t ) NOEXCEPT;
		template< typename U > Ref( U* ref ) NOEXCEPT;
		template< typename U > Ref( U* ref, int ) NOEXCEPT;
		template< typename U > Ref( Ref< U > const& other ) NOEXCEPT;
		template< typename U > Ref( Ref< U >&& other ) NOEXCEPT;
		Ref( Ref< T > const& other ) NOEXCEPT;
		Ref( Ref< T >&& other ) NOEXCEPT;
		~Ref() NOEXCEPT;
		T* possess( std::nullptr_t ) NOEXCEPT;
		template< typename U > T* possess( U* ref ) NOEXCEPT;
		T* assign( std::nullptr_t ) NOEXCEPT;
		template< typename U > T* assign( U* ref ) NOEXCEPT;
		Ref< T >& operator=( std::nullptr_t ) NOEXCEPT;
		template< typename U > Ref< T >& operator=( U* ref ) NOEXCEPT;
		template< typename U > Ref< T >& operator=( Ref< U > const& other ) NOEXCEPT;
		template< typename U > Ref< T >& operator=( Ref< U >&& other ) NOEXCEPT;
		Ref< T >& operator=( Ref< T > const& other ) NOEXCEPT;
		Ref< T >& operator=( Ref< T >&& other ) NOEXCEPT;
		T& operator*() const;
		T* operator->() const;
		operator T*() const NOEXCEPT;
		operator bool() const NOEXCEPT;
		template< typename U > bool operator==( Ref< U > const& other ) const NOEXCEPT;
		template< typename U > bool operator!=( Ref< U > const& other ) const NOEXCEPT;
		template< typename U > bool operator<( Ref< U > const& other ) const NOEXCEPT;
		template< typename U > bool operator<=( Ref< U > const& other ) const NOEXCEPT;
		template< typename U > bool operator>( Ref< U > const& other ) const NOEXCEPT;
		template< typename U > bool operator>=( Ref< U > const& other ) const NOEXCEPT;
		template< typename ...Ts >
		void emplace( Ts&& ...args );
		template< typename ...Ts >
		static Ref< T > create( Ts&& ...args );
	};

	template< typename T >
	Ref< T >::Ref() NOEXCEPT
		: m_ref( nullptr )
	{
	}

	template< typename T >
	Ref< T >::Ref( std::nullptr_t ) NOEXCEPT
		: m_ref( nullptr )
	{
	}

	template< typename T >
	template< typename U > Ref< T >::Ref( U* ref ) NOEXCEPT
		: m_ref( nullptr )
	{
		assign( ref );
	}

	template< typename T >
	template< typename U > Ref< T >::Ref( U* ref, int ) NOEXCEPT
		: m_ref( ref )
	{
	}

	template< typename T >
	template< typename U > Ref< T >::Ref( Ref< U > const& other ) NOEXCEPT
		: m_ref( nullptr )
	{
		U* ref = other.m_ref.load( std::memory_order_relaxed );
		assign( ref );
	}

	template< typename T >
	template< typename U > Ref< T >::Ref( Ref< U >&& other ) NOEXCEPT
		: m_ref( nullptr )
	{
		U* ref = other.m_ref.exchange( nullptr, std::memory_order_relaxed );
		possess( ref );
	}

	template< typename T >
	Ref< T >::Ref( Ref< T > const& other ) NOEXCEPT
		: m_ref( nullptr )
	{
		T* ref = other.m_ref.load( std::memory_order_relaxed );
		assign( ref );
	}

	template< typename T >
	Ref< T >::Ref( Ref< T >&& other ) NOEXCEPT
		: m_ref( nullptr )
	{
		T* ref = other.m_ref.exchange( nullptr, std::memory_order_relaxed );
		possess( ref );
	}

	template< typename T >
	Ref< T >::~Ref() NOEXCEPT
	{
		T* oldref = m_ref.exchange( nullptr, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
	}

	template< typename T >
	T* Ref< T >::possess( std::nullptr_t ) NOEXCEPT
	{
		T* oldref;
		oldref = m_ref.exchange( nullptr, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		return oldref;
	}

	template< typename T >
	template< typename U > T* Ref< T >::possess( U* ref ) NOEXCEPT
	{
		T* oldref;
		oldref = m_ref.exchange( ref, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		return oldref;
	}

	template< typename T >
	T* Ref< T >::assign( std::nullptr_t ) NOEXCEPT
	{
		T* oldref = m_ref.exchange( nullptr, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		return oldref;
	}

	template< typename T >
	template< typename U > T* Ref< T >::assign( U* ref ) NOEXCEPT
	{
		if( ref )
		{
			ref->addref();
		}
		T* oldref = m_ref.exchange( ref, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		return oldref;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( std::nullptr_t ) NOEXCEPT
	{
		assign( nullptr );
		return *this;
	}

	template< typename T >
	template< typename U > Ref< T >& Ref< T >::operator=( U* ref ) NOEXCEPT
	{
		assign( ref );
		return *this;
	}

	template< typename T >
	template< typename U > Ref< T >& Ref< T >::operator=( Ref< U > const& other ) NOEXCEPT
	{
		U* ref = other.m_ref.load( std::memory_order_relaxed );
		assign( ref );
		return *this;
	}

	template< typename T >
	template< typename U > Ref< T >& Ref< T >::operator=( Ref< U >&& other ) NOEXCEPT
	{
		U* ref = other.m_ref.exchange( nullptr, std::memory_order_relaxed );
		possess( ref );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( Ref< T > const& other ) NOEXCEPT
	{
		return operator=< T >( other );
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( Ref< T >&& other ) NOEXCEPT
	{
		return operator=< T >( std::move( other ) );
	}

	template< typename T >
	T& Ref< T >::operator*() const
	{
		T* ref = m_ref.load( std::memory_order_relaxed );
		if( ref )
		{
			return *ref;
		}
		else
		{
			throw std::runtime_error( "dereferencing an empty Ref" );
		}
	}

	template< typename T >
	T* Ref< T >::operator->() const
	{
		return &( operator*() );
	}

	template< typename T >
	Ref< T >::operator T*() const NOEXCEPT
	{
		T* ref = m_ref.load( std::memory_order_relaxed );
		return ref;
	}

	template< typename T >
	Ref< T >::operator bool() const NOEXCEPT
	{
		T* ref = m_ref.load( std::memory_order_relaxed );
		return ref != nullptr;
	}

	template< typename T >
	template< typename U > bool Ref< T >::operator==( Ref< U > const& other ) const NOEXCEPT
	{
		return m_ref.load( std::memory_order_relaxed ) ==
			other.m_ref.load( std::memory_order_relaxed );
	}

	template< typename T >
	template< typename U > bool Ref< T >::operator!=( Ref< U > const& other ) const NOEXCEPT
	{
		return !( *this == other );
	}

	template< typename T >
	template< typename U > bool Ref< T >::operator<( Ref< U > const& other ) const NOEXCEPT
	{
		return m_ref.load( std::memory_order_relaxed ) <
			other.m_ref.load( std::memory_order_relaxed );
	}

	template< typename T >
	template< typename U > bool Ref< T >::operator<=( Ref< U > const& other ) const NOEXCEPT
	{
		return !( other < *this );
	}

	template< typename T >
	template< typename U > bool Ref< T >::operator>( Ref< U > const& other ) const NOEXCEPT
	{
		return other < *this;
	}

	template< typename T >
	template< typename U > bool Ref< T >::operator>=( Ref< U > const& other ) const NOEXCEPT
	{
		return !( *this < other );
	}

	template< typename T >
	template< typename ...Ts >
	void Ref< T >::emplace( Ts&& ...args )
	{
		possess( new T( std::forward< Ts >( args )... ) );
	}

	template< typename T >
	template< typename ...Ts >
	Ref< T > Ref< T >::create( Ts&& ...args )
	{
		return Ref< T >( new T( std::forward< Ts >( args )... ), 0 );
	}
}

#endif
