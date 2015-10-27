#ifndef UTILS_REFOBJECT_HPP__
#define UTILS_REFOBJECT_HPP__

#include <cstdio>
#include <atomic>

#define DEFINE_REFID( type, ptype, refid ) \
	namespace utils { \
		template<> struct RefId< type > { \
			enum e { \
				id = int( refid ) \
			}; \
			typedef ptype parent; \
		}; \
	}

namespace utils {
	template< typename T >
	struct RefId {
		enum e {id = 0};
		typedef void parent;
	};
	
	template< typename T >
	bool queryobject( T* source, int id, void** target ) {
		if( id == RefId< T >::id ) {
			*target = static_cast< T* >( source );
			source->addref();
			return true;
		} else {
			return queryobject< typename RefId< T >::parent >( source, id, target );
		}
	}
	
	class RefObject {
	protected:
		mutable std::atomic< int > m_refcount;
		
	public:
		int addref() const noexcept;
		int release() const noexcept;
		int refcount() const noexcept;
		virtual bool query( int id, void** target );
		RefObject() noexcept;
		RefObject( RefObject const& other ) noexcept;
		RefObject( RefObject&& other ) noexcept;
		virtual ~RefObject() noexcept;
		virtual void destroy();
		RefObject& operator=( RefObject const& other ) noexcept;
		RefObject& operator=( RefObject&& other ) noexcept;
	};
}

DEFINE_REFID( utils::RefObject, void, 0x487e69d6 )

namespace utils {
	inline int RefObject::addref() const noexcept {
		int rc = m_refcount.fetch_add( 1, std::memory_order_relaxed ) + 1;
// 		LOG( "((RefObject*) %#10x)->addref()  - %i", uint32_t( this ), rc );
// 		if( rc == 1 ) {
// 			LOG( "RefObject is brought undead! 0x%x", uint32_t( this ) );
// 		}
		return rc;
	}
	
	inline int RefObject::release() const noexcept {
		int rc = m_refcount.fetch_sub( 1, std::memory_order_relaxed ) - 1;
// 		LOG( "((RefObject*) %#10x)->release() - %i", uint32_t( this ), rc );
		if( rc == 0 ) {
			const_cast< RefObject* >( this )->destroy();
		}
		return rc;
	}
	
	inline int RefObject::refcount() const noexcept {
		return m_refcount.load( std::memory_order_relaxed );
	}
	
// 	template<> bool queryobject< RefObject >( RefObject* source, int id, void** target );
	template<> bool queryobject< void >( void* source, int id, void** target );
		
	template< typename T, typename S >
	T* refptr_cast( S* s ) {
		if( s ) {
			T* result;
			s->query( RefId< T >::id, ( void** )&result );
			return result;
		} else {
			return 0;
		}
	}
}

extern "C" {
	int refobject_addref( utils::RefObject* ro ) noexcept;
	int refobject_release( utils::RefObject* ro ) noexcept;
	utils::RefObject* refobject_refcast( utils::RefObject* ro, int id ) noexcept;
}

#endif
