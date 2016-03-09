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
	class RefBase
	{
	private:
		std::atomic< RefObject* > m_ref;
		RefObject* possess_silent( RefObject* ref ) NOEXCEPT;
		RefObject* assign_silent( RefObject* ref ) NOEXCEPT;

	public:
		RefBase() NOEXCEPT;
		RefBase( RefObject* ref, int ) NOEXCEPT;
		RefBase( RefObject* ref ) NOEXCEPT;
		RefBase( RefBase&& other ) NOEXCEPT;
		RefBase( RefBase const& other ) NOEXCEPT;
		~RefBase() NOEXCEPT;
		RefObject* possess( RefObject* ref ) NOEXCEPT;
		RefObject* assign( RefObject* ref ) NOEXCEPT;
		RefBase& assign( RefBase&& other ) NOEXCEPT;
		RefBase& assign( RefBase const& other ) NOEXCEPT;
		RefObject* get() const NOEXCEPT;
		RefObject& deref() const;
		bool operator==( RefBase const& other );
		bool operator!=( RefBase const& other );
	};

	template< typename T >
	class Ref: public RefBase
	{
	public:
		Ref() NOEXCEPT;
		Ref( std::nullptr_t ) NOEXCEPT;
		Ref( T* ref ) NOEXCEPT;
		Ref( T* ref, int ) NOEXCEPT;
		Ref( Ref< T > const& other ) NOEXCEPT;
		Ref( Ref< T >&& other ) NOEXCEPT;
		~Ref() NOEXCEPT;
		Ref& operator=( std::nullptr_t ) NOEXCEPT;
		Ref& operator=( T* ref ) NOEXCEPT;
		Ref& operator=( Ref< T > const& other ) NOEXCEPT;
		Ref& operator=( Ref< T >&& other ) NOEXCEPT;
		T& operator*() const;
		T* operator->() const;
		operator T*() const NOEXCEPT;

		template< typename ...Ts >
		static Ref< T > create( Ts&& ...args );
	};

	/*template<>
	class Ref< RefObject >: public RefBase
	{
	public:
		Ref() NOEXCEPT;
		Ref( RefObject* ref ) NOEXCEPT;
		Ref( RefObject* ref, int ) NOEXCEPT;
		Ref( Ref const& other ) NOEXCEPT;
		Ref( RefBase const& other ) NOEXCEPT;
		Ref( Ref&& other ) NOEXCEPT;
		Ref( RefBase&& other ) NOEXCEPT;
		~Ref() NOEXCEPT;
		Ref& operator=( RefObject* ref ) NOEXCEPT;
		Ref& operator=( Ref const& other ) NOEXCEPT;
		Ref& operator=( RefBase const& other ) NOEXCEPT;
		Ref& operator=( Ref&& other ) NOEXCEPT;
		Ref& operator=( RefBase&& other ) NOEXCEPT;
		RefObject& operator*() const;
		RefObject* operator->() const;
		operator RefObject*() const NOEXCEPT;
	};*/

	template< typename T >
	Ref< T >::Ref() NOEXCEPT
		: RefBase()
	{
	}

	template< typename T >
	Ref< T >::Ref( std::nullptr_t ) NOEXCEPT
		: RefBase( ( RefObject* )0 )
	{
	}

	template< typename T >
	Ref< T >::Ref( T* ref ) NOEXCEPT
		: RefBase( ( RefObject* )ref )
	{
	}

	template< typename T >
	Ref< T >::Ref( T* ref, int ) NOEXCEPT
		: RefBase( ( RefObject* )ref, 0 )
	{
	}

	template< typename T >
	Ref< T >::Ref( Ref< T > const& other ) NOEXCEPT
		: RefBase( other )
	{
	}

	template< typename T >
	Ref< T >::Ref( Ref< T >&& other ) NOEXCEPT
		: RefBase( std::move( std::move( ( RefBase&& )other ) ) )
	{
	}

	template< typename T >
	Ref< T >::~Ref() NOEXCEPT
	{
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( std::nullptr_t ) NOEXCEPT
	{
		RefBase::assign( ( RefObject* )0 );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( T* ref ) NOEXCEPT
	{
		RefBase::assign( ( RefObject* )ref );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( Ref< T > const& other ) NOEXCEPT
	{
		RefBase::assign( other );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( Ref< T >&& other ) NOEXCEPT
	{
		RefBase::assign( std::move( ( RefBase&& )other ) );
		return *this;
	}

	template< typename T >
	T& Ref< T >::operator*() const
	{
		return ( T& )RefBase::deref();
	}

	template< typename T >
	T* Ref< T >::operator->() const
	{
		return ( T* )&RefBase::deref();
	}

	template< typename T >
	Ref< T >::operator T*() const NOEXCEPT
	{
		return ( T* )RefBase::get();
	}

	template< typename T >
	template< typename ...Ts >
	Ref< T > Ref< T >::create( Ts&& ...args )
	{
		return Ref< T >( new T( std::forward< Ts >( args )... ), 0 );
	}
}

#endif
