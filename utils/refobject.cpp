// #define COUNT_OBJECTS

#include "refobject.hpp"
#include "console.hpp"
#include "cbase.hpp"
#include <cinttypes>

#if defined( _WIN64 )
#define PRIPTR "#18" PRIxPTR
#else
#define PRIPTR "#10" PRIxPTR
#endif

namespace utils {
#ifdef COUNT_OBJECTS
	std::atomic< int > rocount( 0 );
#endif

	RefObject::RefObject() noexcept
		: m_refcount( 1 )
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
	}

	RefObject::RefObject( RefObject const& other ) noexcept
		: m_refcount( 1 )
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
	}

	RefObject::RefObject( RefObject&& other ) noexcept
		: m_refcount( 1 )
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
	}

	RefObject::~RefObject() noexcept
	{
// 		LOG( "((RefObject*) %" PRIPTR ")->~RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "delete refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_sub( 1, std::memory_order_relaxed ) - 1 );
#endif
	}

	void RefObject::destroy()
	{
		delete this;
	}

	RefObject& RefObject::operator=( RefObject const& other ) noexcept {
		return *this;
	}

	RefObject& RefObject::operator=( RefObject&& other ) noexcept {
		return *this;
	}

	extern "C"
	{
		int utils_refobject_addref( RefObject* ro ) noexcept
		{
		CBASE_PROTECT(
			if( ro ) {
				return ro->addref();
			} else {
				return -1;
			}
		)
		}

		int utils_refobject_release( RefObject* ro ) noexcept
		{
		CBASE_PROTECT(
			if( ro ) {
				return ro->release();
			} else {
				return -1;
			}
		)
		}
	}
}
