#pragma once

#include <common.hpp>
#include <atomic>
#include <stdexcept>
#include <utility>
#include <cstddef>

template< typename T >
void addref( T&& ptr )
{
	if( ptr )
	{
		ptr->AddRef();
	}
}

template< typename T >
void release( T&& ptr )
{
	if( ptr )
	{
		ptr->Release();
	}
}

template< typename T >
class Ref
{
public:
	template< typename U > friend class Ref;

private:
	T* m_ref;

public:
	Ref( std::nullptr_t = nullptr ) NOEXCEPT;
	template< typename U > Ref( U* ref ) NOEXCEPT;
	template< typename U > Ref( U* ref, int ) NOEXCEPT;
	template< typename U > Ref( Ref< U > const& other ) NOEXCEPT;
	template< typename U > Ref( Ref< U >&& other ) NOEXCEPT;
	Ref( Ref< T > const& other ) NOEXCEPT;
	Ref( Ref< T >&& other ) NOEXCEPT;
	~Ref() NOEXCEPT;
	void possess( std::nullptr_t ) NOEXCEPT;
	template< typename U > void possess( U* ref ) NOEXCEPT;
	void assign( std::nullptr_t ) NOEXCEPT;
	template< typename U > void assign( U* ref ) NOEXCEPT;
	T* detach() NOEXCEPT;
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
	template< typename U > bool operator==(
		Ref< U > const& other ) const NOEXCEPT;
	template< typename U > bool operator!=(
		Ref< U > const& other ) const NOEXCEPT;
	template< typename U > bool operator<(
		Ref< U > const& other ) const NOEXCEPT;
	template< typename U > bool operator<=(
		Ref< U > const& other ) const NOEXCEPT;
	template< typename U > bool operator>(
		Ref< U > const& other ) const NOEXCEPT;
	template< typename U > bool operator>=(
		Ref< U > const& other ) const NOEXCEPT;
	template< typename ...Ts >
	void emplace( Ts&& ...args );
	template< typename ...Ts >
	static Ref< T > create( Ts&& ...args );
};

template< typename T >
class AtomicRef
{
public:
	template< typename U > friend class AtomicRef;

private:
	std::atomic< ptrdiff_t > m_use;
	std::atomic< T* > m_ref;

public:
	AtomicRef( std::nullptr_t = nullptr ) NOEXCEPT;
	template< typename U > AtomicRef( U* ptr ) NOEXCEPT;
	template< typename U > AtomicRef( Ref< U > const& ref ) NOEXCEPT;
	template< typename U > AtomicRef( Ref< U >&& ref ) NOEXCEPT;
	AtomicRef( AtomicRef< T > const& other ) = delete;
	AtomicRef( AtomicRef< T >&& other ) = delete;
	~AtomicRef() NOEXCEPT;
	void assign( std::nullptr_t = nullptr ) NOEXCEPT;
	template< typename U > void assign( U* ptr ) NOEXCEPT;
	template< typename U > void assign( Ref< U > const& ref ) NOEXCEPT;
	template< typename U > void assign( Ref< U >&& ref ) NOEXCEPT;
	Ref< T > getref() NOEXCEPT;
	Ref< T > detachref() NOEXCEPT;
	AtomicRef< T >& operator=( AtomicRef< T > const& other ) = delete;
	AtomicRef< T >& operator=( AtomicRef< T >&& other ) = delete;
};

template< typename T >
Ref< T >::Ref( std::nullptr_t ) NOEXCEPT
	: m_ref( nullptr )
{
}

template< typename T >
template< typename U > Ref< T >::Ref( U* ref ) NOEXCEPT
	: m_ref( ref )
{
	::addref( ref );
}

template< typename T >
template< typename U > Ref< T >::Ref( U* ref, int ) NOEXCEPT
	: m_ref( ref )
{
}

template< typename T >
template< typename U > Ref< T >::Ref( Ref< U > const& other ) NOEXCEPT
	: m_ref( other.m_ref )
{
	::addref( m_ref );
}

template< typename T >
template< typename U > Ref< T >::Ref( Ref< U >&& other ) NOEXCEPT
	: m_ref( other.m_ref )
{
	other.m_ref = nullptr;
}

template< typename T >
Ref< T >::Ref( Ref< T > const& other ) NOEXCEPT
	: m_ref( other.m_ref )
{
	::addref( other.m_ref );
}

template< typename T >
Ref< T >::Ref( Ref< T >&& other ) NOEXCEPT
	: m_ref( other.m_ref )
{
	other.m_ref = 0;
}

template< typename T >
Ref< T >::~Ref() NOEXCEPT
{
	::release( m_ref );
}

template< typename T >
void Ref< T >::possess( std::nullptr_t ) NOEXCEPT
{
	T* oldref = m_ref;
	m_ref = nullptr;
	::release( oldref );
}

template< typename T >
template< typename U > void Ref< T >::possess( U* ref ) NOEXCEPT
{
	T* oldref = m_ref;
	m_ref = ref;
	::release( oldref );
}

template< typename T >
void Ref< T >::assign( std::nullptr_t ) NOEXCEPT
{
	T* oldref = m_ref;
	m_ref = nullptr;
	::release( oldref );
}

template< typename T >
template< typename U > void Ref< T >::assign( U* ref ) NOEXCEPT
{
	::addref( ref );
	T* oldref = m_ref;
	m_ref = ref;
	::release( oldref );
}

template< typename T >
T* Ref< T >::detach() NOEXCEPT
{
	T* oldref = m_ref;
	m_ref = nullptr;
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
template< typename U > Ref< T >& Ref< T >::operator=(
	Ref< U > const& other ) NOEXCEPT
{
	U* ref = other.m_ref;
	assign( ref );
	return *this;
}

template< typename T >
template< typename U > Ref< T >& Ref< T >::operator=(
	Ref< U >&& other ) NOEXCEPT
{
	U* ref = other.m_ref;
	other.m_ref = nullptr;
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
	T* ref = m_ref;
#if defined( MIST_DEBUG )
	if( !ref )
	{
		throw std::runtime_error( "dereferencing an empty Ref" );
	}
#endif
	return *ref;
}

template< typename T >
T* Ref< T >::operator->() const
{
	return &( operator*() );
}

template< typename T >
Ref< T >::operator T*() const NOEXCEPT
{
	return m_ref;
}

template< typename T >
Ref< T >::operator bool() const NOEXCEPT
{
	return m_ref != nullptr;
}

template< typename T >
template< typename U > bool Ref< T >::operator==(
	Ref< U > const& other ) const NOEXCEPT
{
	return m_ref == other.m_ref;
}

template< typename T >
template< typename U > bool Ref< T >::operator!=(
	Ref< U > const& other ) const NOEXCEPT
{
	return !( *this == other );
}

template< typename T >
template< typename U > bool Ref< T >::operator<(
	Ref< U > const& other ) const NOEXCEPT
{
	return m_ref < other.m_ref;
}

template< typename T >
template< typename U > bool Ref< T >::operator<=(
	Ref< U > const& other ) const NOEXCEPT
{
	return !( other < *this );
}

template< typename T >
template< typename U > bool Ref< T >::operator>(
	Ref< U > const& other ) const NOEXCEPT
{
	return other < *this;
}

template< typename T >
template< typename U > bool Ref< T >::operator>=(
	Ref< U > const& other ) const NOEXCEPT
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

template< typename T >
AtomicRef< T >::AtomicRef( std::nullptr_t ) NOEXCEPT
	: m_use( 0 )
	, m_ref( nullptr )
{
}

template< typename T >
template< typename U > AtomicRef< T >::AtomicRef( U* ptr ) NOEXCEPT
	: m_use( 0 )
	, m_ref( ptr )
{
	::addref( ptr );
}

template< typename T >
template< typename U > AtomicRef< T >::AtomicRef( Ref< U > const& ref ) NOEXCEPT
	: m_use( 0 )
	, m_ref( ( U* )ref )
{
	::addref( ( U* )ref );
}

template< typename T >
template< typename U > AtomicRef< T >::AtomicRef( Ref< U >&& ref ) NOEXCEPT
	: m_use( 0 )
	, m_ref( ref.detach() )
{
}

template< typename T >
AtomicRef< T >::~AtomicRef() NOEXCEPT
{
	ASSERT( m_use.load( std::memory_order_relaxed ) == 0 );
	T* ptr = m_ref.load( std::memory_order_relaxed );
	::release( ptr );
}

template< typename T >
void AtomicRef< T >::assign( std::nullptr_t ) NOEXCEPT
{
	T* oldref = m_ref.exchange( nullptr, std::memory_order_acq_rel );
	if( oldref )
	{
		while( m_use.load( std::memory_order_acquire ) != 0 )
		{
		}
		::release( oldref );
	}
}

template< typename T >
template< typename U > void AtomicRef< T >::assign(
	U* ptr ) NOEXCEPT
{
	::addref( ptr );
	T* oldref = m_ref.exchange( ptr, std::memory_order_acq_rel );
	if( oldref )
	{
		while( m_use.load( std::memory_order_acquire ) != 0 )
		{
		}
		::release( oldref );
	}
}

template< typename T >
template< typename U > void AtomicRef< T >::assign(
	Ref< U > const& ref ) NOEXCEPT
{
	U* ptr = ref;
	::addref( ptr );
	T* oldref = m_ref.exchange( ptr, std::memory_order_acq_rel );
	if( oldref )
	{
		while( m_use.load( std::memory_order_acquire ) != 0 )
		{
		}
		::release( oldref );
	}
}

template< typename T >
template< typename U > void AtomicRef< T >::assign(
	Ref< U >&& ref ) NOEXCEPT
{
	U* ptr = ref.detach();
	T* oldref = m_ref.exchange( ptr, std::memory_order_acq_rel );
	if( oldref )
	{
		while( m_use.load( std::memory_order_acquire ) != 0 )
		{
		}
		::release( oldref );
	}
}

template< typename T >
Ref< T > AtomicRef< T >::getref() NOEXCEPT
{
	m_use.fetch_add( 1, std::memory_order_acquire );
	Ref< T > result( m_ref.load( std::memory_order_acquire ) );
	m_use.fetch_sub( 1, std::memory_order_release );
	return result;
}

template< typename T >
Ref< T > AtomicRef< T >::detachref() NOEXCEPT
{
	Ref< T > result( m_ref.exchange( nullptr, std::memory_order_acq_rel ), 0 );
	return result;
}
