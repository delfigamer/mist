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
	Ref( std::nullptr_t = nullptr ) noexcept;
	template< typename U > Ref( U* ref ) noexcept;
	template< typename U > Ref( U* ref, int ) noexcept;
	template< typename U > Ref( Ref< U > const& other ) noexcept;
	template< typename U > Ref( Ref< U >&& other ) noexcept;
	Ref( Ref< T > const& other ) noexcept;
	Ref( Ref< T >&& other ) noexcept;
	~Ref() noexcept;
	void possess( std::nullptr_t ) noexcept;
	template< typename U > void possess( U* ref ) noexcept;
	void assign( std::nullptr_t ) noexcept;
	template< typename U > void assign( U* ref ) noexcept;
	T* detach() noexcept;
	T* share() noexcept;
	Ref< T >& operator=( std::nullptr_t ) noexcept;
	template< typename U > Ref< T >& operator=( U* ref ) noexcept;
	template< typename U > Ref< T >& operator=( Ref< U > const& other ) noexcept;
	template< typename U > Ref< T >& operator=( Ref< U >&& other ) noexcept;
	Ref< T >& operator=( Ref< T > const& other ) noexcept;
	Ref< T >& operator=( Ref< T >&& other ) noexcept;
	T& operator*() const;
	T* operator->() const;
	operator T*() const noexcept;
	operator bool() const noexcept;
	template< typename U > bool operator==(
		Ref< U > const& other ) const noexcept;
	template< typename U > bool operator!=(
		Ref< U > const& other ) const noexcept;
	template< typename U > bool operator<(
		Ref< U > const& other ) const noexcept;
	template< typename U > bool operator<=(
		Ref< U > const& other ) const noexcept;
	template< typename U > bool operator>(
		Ref< U > const& other ) const noexcept;
	template< typename U > bool operator>=(
		Ref< U > const& other ) const noexcept;
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
	AtomicRef( std::nullptr_t = nullptr ) noexcept;
	template< typename U > AtomicRef( U* ptr ) noexcept;
	template< typename U > AtomicRef( Ref< U > const& ref ) noexcept;
	template< typename U > AtomicRef( Ref< U >&& ref ) noexcept;
	AtomicRef( AtomicRef< T > const& other ) = delete;
	AtomicRef( AtomicRef< T >&& other ) = delete;
	~AtomicRef() noexcept;
	void assign( std::nullptr_t = nullptr ) noexcept;
	template< typename U > void assign( U* ptr ) noexcept;
	template< typename U > void assign( Ref< U > const& ref ) noexcept;
	template< typename U > void assign( Ref< U >&& ref ) noexcept;
	Ref< T > getref() noexcept;
	Ref< T > detachref() noexcept;
	AtomicRef< T >& operator=( AtomicRef< T > const& other ) = delete;
	AtomicRef< T >& operator=( AtomicRef< T >&& other ) = delete;
};

template< typename T >
Ref< T >::Ref( std::nullptr_t ) noexcept
	: m_ref( nullptr )
{
}

template< typename T >
template< typename U > Ref< T >::Ref( U* ref ) noexcept
	: m_ref( ref )
{
	::addref( ref );
}

template< typename T >
template< typename U > Ref< T >::Ref( U* ref, int ) noexcept
	: m_ref( ref )
{
}

template< typename T >
template< typename U > Ref< T >::Ref( Ref< U > const& other ) noexcept
	: m_ref( other.m_ref )
{
	::addref( m_ref );
}

template< typename T >
template< typename U > Ref< T >::Ref( Ref< U >&& other ) noexcept
	: m_ref( other.m_ref )
{
	other.m_ref = nullptr;
}

template< typename T >
Ref< T >::Ref( Ref< T > const& other ) noexcept
	: m_ref( other.m_ref )
{
	::addref( other.m_ref );
}

template< typename T >
Ref< T >::Ref( Ref< T >&& other ) noexcept
	: m_ref( other.m_ref )
{
	other.m_ref = 0;
}

template< typename T >
Ref< T >::~Ref() noexcept
{
	::release( m_ref );
}

template< typename T >
void Ref< T >::possess( std::nullptr_t ) noexcept
{
	T* oldref = m_ref;
	m_ref = nullptr;
	::release( oldref );
}

template< typename T >
template< typename U > void Ref< T >::possess( U* ref ) noexcept
{
	T* oldref = m_ref;
	m_ref = ref;
	::release( oldref );
}

template< typename T >
void Ref< T >::assign( std::nullptr_t ) noexcept
{
	T* oldref = m_ref;
	m_ref = nullptr;
	::release( oldref );
}

template< typename T >
template< typename U > void Ref< T >::assign( U* ref ) noexcept
{
	::addref( ref );
	T* oldref = m_ref;
	m_ref = ref;
	::release( oldref );
}

template< typename T >
T* Ref< T >::detach() noexcept
{
	T* oldref = m_ref;
	m_ref = nullptr;
	return oldref;
}

template< typename T >
T* Ref< T >::share() noexcept
{
	T* oldref = m_ref;
	::addref( oldref );
	return oldref;
}

template< typename T >
Ref< T >& Ref< T >::operator=( std::nullptr_t ) noexcept
{
	assign( nullptr );
	return *this;
}

template< typename T >
template< typename U > Ref< T >& Ref< T >::operator=( U* ref ) noexcept
{
	assign( ref );
	return *this;
}

template< typename T >
template< typename U > Ref< T >& Ref< T >::operator=(
	Ref< U > const& other ) noexcept
{
	U* ref = other.m_ref;
	assign( ref );
	return *this;
}

template< typename T >
template< typename U > Ref< T >& Ref< T >::operator=(
	Ref< U >&& other ) noexcept
{
	U* ref = other.m_ref;
	other.m_ref = nullptr;
	possess( ref );
	return *this;
}

template< typename T >
Ref< T >& Ref< T >::operator=( Ref< T > const& other ) noexcept
{
	return operator=< T >( other );
}

template< typename T >
Ref< T >& Ref< T >::operator=( Ref< T >&& other ) noexcept
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
Ref< T >::operator T*() const noexcept
{
	return m_ref;
}

template< typename T >
Ref< T >::operator bool() const noexcept
{
	return m_ref != nullptr;
}

template< typename T >
template< typename U > bool Ref< T >::operator==(
	Ref< U > const& other ) const noexcept
{
	return m_ref == other.m_ref;
}

template< typename T >
template< typename U > bool Ref< T >::operator!=(
	Ref< U > const& other ) const noexcept
{
	return !( *this == other );
}

template< typename T >
template< typename U > bool Ref< T >::operator<(
	Ref< U > const& other ) const noexcept
{
	return m_ref < other.m_ref;
}

template< typename T >
template< typename U > bool Ref< T >::operator<=(
	Ref< U > const& other ) const noexcept
{
	return !( other < *this );
}

template< typename T >
template< typename U > bool Ref< T >::operator>(
	Ref< U > const& other ) const noexcept
{
	return other < *this;
}

template< typename T >
template< typename U > bool Ref< T >::operator>=(
	Ref< U > const& other ) const noexcept
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
AtomicRef< T >::AtomicRef( std::nullptr_t ) noexcept
	: m_use( 0 )
	, m_ref( nullptr )
{
}

template< typename T >
template< typename U > AtomicRef< T >::AtomicRef( U* ptr ) noexcept
	: m_use( 0 )
	, m_ref( ptr )
{
	::addref( ptr );
}

template< typename T >
template< typename U > AtomicRef< T >::AtomicRef( Ref< U > const& ref ) noexcept
	: m_use( 0 )
	, m_ref( ( U* )ref )
{
	::addref( ( U* )ref );
}

template< typename T >
template< typename U > AtomicRef< T >::AtomicRef( Ref< U >&& ref ) noexcept
	: m_use( 0 )
	, m_ref( ref.detach() )
{
}

template< typename T >
AtomicRef< T >::~AtomicRef() noexcept
{
	ASSERT( m_use.load( std::memory_order_relaxed ) == 0 );
	T* ptr = m_ref.load( std::memory_order_relaxed );
	::release( ptr );
}

template< typename T >
void AtomicRef< T >::assign( std::nullptr_t ) noexcept
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
	U* ptr ) noexcept
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
	Ref< U > const& ref ) noexcept
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
	Ref< U >&& ref ) noexcept
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
Ref< T > AtomicRef< T >::getref() noexcept
{
	m_use.fetch_add( 1, std::memory_order_acquire );
	Ref< T > result( m_ref.load( std::memory_order_acquire ) );
	m_use.fetch_sub( 1, std::memory_order_release );
	return result;
}

template< typename T >
Ref< T > AtomicRef< T >::detachref() noexcept
{
	T* oldref = m_ref.exchange( nullptr, std::memory_order_acq_rel );
	Ref< T > result( oldref, 0 );
	if( oldref )
	{
		while( m_use.load( std::memory_order_acquire ) != 0 )
		{
		}
	}
	return result;
}
