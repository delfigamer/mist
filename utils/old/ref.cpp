#include <utils/ref.hpp>
#include <atomic>
#include <exception>
#include <stdexcept>
#include <utility>

namespace utils
{
	RefBase::RefBase() NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
	}

	RefBase::RefBase( RefObject* ref ) NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		assign_silent( ref );
	}

	RefBase::RefBase( RefObject* ref, int ) NOEXCEPT
		: m_ref( ref )
	{
	}

	RefBase::RefBase( RefBase&& other ) NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		RefObject* ref = other.m_ref.exchange( 0, std::memory_order_relaxed );
		possess_silent( ref );
	}

	RefBase::RefBase( RefBase const& other ) NOEXCEPT
		: m_ref( ( RefObject* )0 )
	{
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		assign_silent( ref );
	}

	RefBase::~RefBase() NOEXCEPT
	{
		RefObject* oldref = m_ref.exchange( 0, std::memory_order_relaxed );
		if( oldref )
		{
			oldref->release();
		}
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
		return oldref;
	}

	RefObject* RefBase::assign( RefObject* ref ) NOEXCEPT
	{
		RefObject* oldref = assign_silent( ref );
		return oldref;
	}

	RefBase& RefBase::assign( RefBase&& other ) NOEXCEPT
	{
		RefObject* ref = other.m_ref.exchange( 0, std::memory_order_relaxed );
		RefObject* oldref = possess_silent( ref );
		( void )oldref;
		return *this;
	}

	RefBase& RefBase::assign( RefBase const& other ) NOEXCEPT
	{
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		RefObject* oldref = assign_silent( ref );
		( void )oldref;
		return *this;
	}

	RefObject* RefBase::get() const NOEXCEPT
	{
		RefObject* ref = m_ref.load( std::memory_order_relaxed );
		return ref;
	}

	RefObject& RefBase::deref() const
	{
		RefObject* ref = m_ref.load( std::memory_order_relaxed );
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
}
