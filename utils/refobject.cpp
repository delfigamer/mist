// #define COUNT_OBJECTS

#include <utils/refobject.hpp>
#include <utils/console.hpp>
#include <utils/cbase.hpp>
#include <cinttypes>

#if defined( _WIN64 )
#define PRIPTR "#18" PRIxPTR
#else
#define PRIPTR "#10" PRIxPTR
#endif

namespace utils
{
#ifdef COUNT_OBJECTS
	std::atomic< int > rocount( 0 );
#endif

	RefObject::RefObject() NOEXCEPT
		: m_refcount( 1 )
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i",
			uintptr_t( this ),
			rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
	}

	RefObject::RefObject( RefObject const& other ) NOEXCEPT
		: m_refcount( 1 )
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i",
			uintptr_t( this ),
			rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
	}

	RefObject::RefObject( RefObject&& other ) NOEXCEPT
		: m_refcount( 1 )
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i",
			uintptr_t( this ),
			rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
	}

	RefObject::~RefObject() NOEXCEPT
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->~RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "delete refobject %" PRIPTR ", utils::rocount = %i",
			uintptr_t( this ),
			rocount.fetch_sub( 1, std::memory_order_relaxed ) - 1 );
#endif
	}

	void RefObject::destroy()
	{
		delete this;
	}

	RefObject& RefObject::operator=( RefObject const& other ) NOEXCEPT
	{
		return *this;
	}

	RefObject& RefObject::operator=( RefObject&& other ) NOEXCEPT
	{
		return *this;
	}
}
