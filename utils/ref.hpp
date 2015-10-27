#ifndef UTILS_REF_HPP__
#define UTILS_REF_HPP__ 1

#include "refobject.hpp"
#include <atomic>
#include <stdexcept>
#include <exception>
#include <utility>
#include <cstddef>

namespace utils {
	class RefBase {
	private:
		std::atomic< RefObject* > m_ref;
		RefObject* possess_silent( RefObject* ref ) noexcept;
		RefObject* assign_silent( RefObject* ref ) noexcept;

	public:
		RefBase() noexcept;
		RefBase( RefObject* ref, int ) noexcept;
		RefBase( RefObject* ref ) noexcept;
		RefBase( RefBase&& other ) noexcept;
		RefBase( RefBase const& other ) noexcept;
		~RefBase() noexcept;
		RefObject* possess( RefObject* ref ) noexcept;
		RefObject* assign( RefObject* ref ) noexcept;
		RefBase& assign( RefBase&& other ) noexcept;
		RefBase& assign( RefBase const& other ) noexcept;
		RefObject* get() const noexcept;
		RefObject& deref() const;
		bool operator==( RefBase const& other );
		bool operator!=( RefBase const& other );
	};

	template< typename T >
	class Ref: public RefBase {
	public:
		Ref() noexcept;
		Ref( std::nullptr_t ) noexcept;
		Ref( T* ref ) noexcept;
		Ref( T* ref, int ) noexcept;
		Ref( Ref const& other ) noexcept;
		Ref( Ref&& other ) noexcept;
		~Ref() noexcept;
		Ref& operator=( std::nullptr_t ) noexcept;
		Ref& operator=( T* ref ) noexcept;
		Ref& operator=( Ref const& other ) noexcept;
		Ref& operator=( Ref&& other ) noexcept;
		T& operator*() const;
		T* operator->() const;
		operator T*() const noexcept;

		template< typename ...Ts >
		static Ref create( Ts&& ...args );
	};

	template<>
	class Ref< RefObject >: public RefBase {
	public:
		Ref() noexcept;
		Ref( RefObject* ref ) noexcept;
		Ref( RefObject* ref, int ) noexcept;
		Ref( Ref const& other ) noexcept;
		Ref( RefBase const& other ) noexcept;
		Ref( Ref&& other ) noexcept;
		Ref( RefBase&& other ) noexcept;
		~Ref() noexcept;
		Ref& operator=( RefObject* ref ) noexcept;
		Ref& operator=( Ref const& other ) noexcept;
		Ref& operator=( RefBase const& other ) noexcept;
		Ref& operator=( Ref&& other ) noexcept;
		Ref& operator=( RefBase&& other ) noexcept;
		RefObject& operator*() const;
		RefObject* operator->() const;
		operator RefObject*() const noexcept;
	};

	template< typename T >
	Ref< T >::Ref() noexcept
		: RefBase()
	{
	}

	template< typename T >
	Ref< T >::Ref( std::nullptr_t ) noexcept
		: RefBase( ( RefObject* )0 )
	{
	}

	template< typename T >
	Ref< T >::Ref( T* ref ) noexcept
		: RefBase( ( RefObject* )ref )
	{
	}

	template< typename T >
	Ref< T >::Ref( T* ref, int ) noexcept
		: RefBase( ( RefObject* )ref, 0 )
	{
	}

	template< typename T >
	Ref< T >::Ref( Ref const& other ) noexcept
		: RefBase( other )
	{
	}

	template< typename T >
	Ref< T >::Ref( Ref&& other ) noexcept
		: RefBase( std::move( ( RefBase&& )other ) )
	{
	}

	template< typename T >
	Ref< T >::~Ref() noexcept
	{
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( std::nullptr_t ) noexcept
	{
		RefBase::assign( ( RefObject* )0 );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( T* ref ) noexcept
	{
		RefBase::assign( ( RefObject* )ref );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( Ref const& other ) noexcept
	{
		RefBase::assign( other );
		return *this;
	}

	template< typename T >
	Ref< T >& Ref< T >::operator=( Ref&& other ) noexcept
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
	Ref< T >::operator T*() const noexcept
	{
		return ( T* )RefBase::get();
	}

	template< typename T >
	template< typename ...Ts >
	Ref< T > Ref< T >::create( Ts&& ...args )
	{
		return Ref( new T( std::forward< Ts >( args )... ), 0 );
	}
}

#endif
