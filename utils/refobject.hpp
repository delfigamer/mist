#ifndef UTILS_REFOBJECT_HPP__
#define UTILS_REFOBJECT_HPP__

#include <cstdio>
#include <atomic>

namespace utils {
	class RefObject {
	protected:
		mutable std::atomic< int > m_refcount;

	public:
		int addref() const noexcept;
		int release() const noexcept;
		int refcount() const noexcept;
		RefObject() noexcept;
		RefObject( RefObject const& other ) noexcept;
		RefObject( RefObject&& other ) noexcept;
		virtual ~RefObject() noexcept;
		virtual void destroy();
		RefObject& operator=( RefObject const& other ) noexcept;
		RefObject& operator=( RefObject&& other ) noexcept;
	};

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

	extern "C" {
		int utils_refobject_addref( RefObject* ro ) noexcept;
		int utils_refobject_release( RefObject* ro ) noexcept;
	}
}


#endif
