#include <utils/ref.hpp>
#include <utils/console.hpp>
#include <atomic>
#include <exception>
#include <stdexcept>
#include <utility>

#undef LOG
#define LOG( ... )

namespace utils
{
	RefBase::RefBase() NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		LOG( "< Ref: %#10x : %#10x >\tRef()",
			uint32_t( this ), 0 );
	}

	RefBase::RefBase( RefObject* ref ) NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		LOG( "< Ref: %#10x : %#10x >\tRef( %#10x )",
			uint32_t( this ), 0, uint32_t( ref ) );
		assign_silent( ref );
	}

	RefBase::RefBase( RefObject* ref, int ) NOEXCEPT
		: m_ref( ref )
	{
		LOG( "< Ref: %#10x : %#10x >\tRef( %#10x, int() )",
			uint32_t( this ), 0, uint32_t( ref ) );
	}

	RefBase::RefBase( RefBase&& other ) NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		RefObject* ref = other.m_ref.exchange( 0, std::memory_order_relaxed );
		possess_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tRef( std::move( < Ref: %#10x : %#10x > ) )",
			uint32_t( this ), 0, uint32_t( &other ), uint32_t( ref ) );
	}

	RefBase::RefBase( RefBase const& other ) NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tRef( < Ref: %#10x : %#10x > )",
			uint32_t( this ), 0, uint32_t( &other ), uint32_t( ref ) );
		assign_silent( ref );
	}

	RefBase::~RefBase() NOEXCEPT
	{
		RefObject* oldref = m_ref.exchange( 0, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		LOG( "< Ref: %#10x : %#10x >\t~Ref()",
			uint32_t( this ), uint32_t( oldref ) );
	}

	RefObject* RefBase::possess_silent( RefObject* ref ) NOEXCEPT
	{
		RefObject* oldref;
		oldref = m_ref.exchange( ref, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		return oldref;
	}

	RefObject* RefBase::assign_silent( RefObject* ref ) NOEXCEPT
	{
		if( ref )
		{
			ref->addref();
		}
		RefObject* oldref = m_ref.exchange( ref, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
		return oldref;
	}

	RefObject* RefBase::possess( RefObject* ref ) NOEXCEPT
	{
		RefObject* oldref = possess_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tpossess( %#10x )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( ref ) );
		return oldref;
	}

	RefObject* RefBase::assign( RefObject* ref ) NOEXCEPT
	{
		RefObject* oldref = assign_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tassign( %#10x )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( ref ) );
		return oldref;
	}

	RefBase& RefBase::assign( RefBase&& other ) NOEXCEPT
	{
		RefObject* ref = other.m_ref.exchange( 0, std::memory_order_relaxed );
		RefObject* oldref = possess_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tassign( std::move( < Ref: %#10x : %#10x > ) )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( &other ), uint32_t( ref ) );
		( void )oldref;
		return *this;
	}

	RefBase& RefBase::assign( RefBase const& other ) NOEXCEPT
	{
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		RefObject* oldref = assign_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tassign( < Ref: %#10x : %#10x > )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( &other ), uint32_t( ref ) );
		( void )oldref;
		return *this;
	}

	RefObject* RefBase::get() const NOEXCEPT
	{
		RefObject* ref = m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tget()", uint32_t( this ), uint32_t( ref ) );
		return ref;
	}

	RefObject& RefBase::deref() const
	{
		RefObject* ref = m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tderef()", uint32_t( this ), uint32_t( ref ) );
		if( ref )
		{
			return *ref;
		}
		else
		{
			throw std::runtime_error( "dereferencing an empty Ref" );
		}
	}

	bool RefBase::operator==( RefBase const& other )
	{
		return m_ref.load( std::memory_order_relaxed ) == other.m_ref.load( std::memory_order_relaxed );
	}

	bool RefBase::operator!=( RefBase const& other )
	{
		return !( *this == other );
	}

	/*Ref< RefObject >::Ref() NOEXCEPT
		: RefBase()
	{
	}

	Ref< RefObject >::Ref( RefObject* ref ) NOEXCEPT
		: RefBase( ref )
	{
	}

	Ref< RefObject >::Ref( RefObject* ref, int ) NOEXCEPT
		: RefBase( ref, 0 )
	{
	}

	Ref< RefObject >::Ref( Ref< RefObject > const& other ) NOEXCEPT
		: RefBase( other )
	{
	}

	Ref< RefObject >::Ref( RefBase const& other ) NOEXCEPT
		: RefBase( other )
	{
	}

	Ref< RefObject >::Ref( Ref< RefObject >&& other ) NOEXCEPT
		: RefBase( std::move( other ) )
	{
	}

	Ref< RefObject >::Ref( RefBase&& other ) NOEXCEPT
		: RefBase( std::move( other ) )
	{
	}

	Ref< RefObject >::~Ref() NOEXCEPT
	{
	}

	Ref< RefObject >& Ref< RefObject >::operator=( RefObject* ref ) NOEXCEPT
	{
		RefBase::assign( ref );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( Ref< RefObject > const& other ) NOEXCEPT
	{
		RefBase::assign( other );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( RefBase const& other ) NOEXCEPT
	{
		RefBase::assign( other );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( Ref< RefObject >&& other ) NOEXCEPT
	{
		RefBase::assign( std::move( other ) );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( RefBase&& other ) NOEXCEPT
	{
		RefBase::assign( std::move( other ) );
		return *this;
	}

	RefObject& Ref< RefObject >::operator*() const
	{
		return RefBase::deref();
	}

	RefObject* Ref< RefObject >::operator->() const
	{
		return &RefBase::deref();
	}

	Ref< RefObject >::operator RefObject*() const NOEXCEPT
	{
		return RefBase::get();
	}*/
}
