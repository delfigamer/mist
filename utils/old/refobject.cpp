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

	bool RefObject::query( int id, void** target ) {
		return queryobject( this, id, target );
	}

	RefObject::RefObject() noexcept {
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
		m_refcount = 1;
	}

	RefObject::RefObject( RefObject const& other ) noexcept {
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
		m_refcount = 1;
	}

	RefObject::RefObject( RefObject&& other ) noexcept {
// 		LOG( "((RefObject*) %" PRIPTR ")->RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "   new refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_add( 1, std::memory_order_relaxed ) + 1 );
#endif
		m_refcount = 1;
	}

	RefObject::~RefObject() noexcept {
// 		LOG( "((RefObject*) %" PRIPTR ")->~RefObject()", uintptr_t( this ) );
#ifdef COUNT_OBJECTS
		LOG( "delete refobject %" PRIPTR ", utils::rocount = %i", uintptr_t( this ), rocount.fetch_sub( 1, std::memory_order_relaxed ) - 1 );
#endif
	}

	void RefObject::destroy() {
		delete this;
	}

	RefObject& RefObject::operator=( RefObject const& other ) noexcept {
		return *this;
	}

	RefObject& RefObject::operator=( RefObject&& other ) noexcept {
		return *this;
	}

// 	template<>
// 	bool queryobject< RefObject >( RefObject* source, int id, void** target ) {
// 		if( id == RefId< RefObject >::id ) {
// 			*target = static_cast< RefObject* >( source );
// 			source->addref();
// 			return true;
// 		} else {
// 			*target = 0;
// 			return false;
// 		}
// 	}

	template<>
	bool queryobject< void >( void* source, int id, void** target ) {
		*target = 0;
		return false;
	}
}

extern "C" {
	int refobject_addref( utils::RefObject* ro ) noexcept {
		CBASE_PROTECT(
			if( ro ) {
				return ro->addref();
			} else {
				return -1;
			}
		)
	}

	int refobject_release( utils::RefObject* ro ) noexcept {
		CBASE_PROTECT(
			if( ro ) {
				return ro->release();
			} else {
				return -1;
			}
		)
	}

	utils::RefObject* refobject_refcast( utils::RefObject* ro, int id ) noexcept {
		CBASE_PROTECT(
			if( ro ) {
				utils::RefObject* ptr;
				ro->query( id, ( void** )&ptr );
				return ptr;
			} else {
				return 0;
			}
		)
	}
}
