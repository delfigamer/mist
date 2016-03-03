#ifndef UTILS_REFOBJECT_HPP__
#define UTILS_REFOBJECT_HPP__ 1

#include <common.hpp>
#include <cstdio>
#include <atomic>

namespace utils
{
	R_CLASS( name = refobject )
	class RefObject
	{
	protected:
		mutable std::atomic< int > m_refcount;

	public:
		int addref() const NOEXCEPT;
		int release() const NOEXCEPT;
		int refcount() const NOEXCEPT;
		R_METHOD( name = addref, noluamethod ) void vaddref() const NOEXCEPT
		{
			addref();
		}
		R_METHOD( name = release, release ) void vrelease() const NOEXCEPT
		{
			release();
		}
		RefObject() NOEXCEPT;
		RefObject( RefObject const& other ) NOEXCEPT;
		RefObject( RefObject&& other ) NOEXCEPT;
		virtual ~RefObject() NOEXCEPT;
		virtual void destroy();
		RefObject& operator=( RefObject const& other ) NOEXCEPT;
		RefObject& operator=( RefObject&& other ) NOEXCEPT;
	};

	inline int RefObject::addref() const NOEXCEPT
	{
		int rc = m_refcount.fetch_add( 1, std::memory_order_relaxed ) + 1;
// 		LOG( "((RefObject*) %#10x)->addref()  - %i", uint32_t( this ), rc );
// 		if( rc == 1 ) {
// 			LOG( "RefObject is brought undead! 0x%x", uint32_t( this ) );
// 		}
		return rc;
	}

	inline int RefObject::release() const NOEXCEPT
	{
		if( !this )
		{
			return -1;
		}
		int rc = m_refcount.fetch_sub( 1, std::memory_order_relaxed ) - 1;
// 		LOG( "((RefObject*) %#10x)->release() - %i", uint32_t( this ), rc );
		if( rc == 0 )
		{
			const_cast< RefObject* >( this )->destroy();
		}
		return rc;
	}

	inline int RefObject::refcount() const NOEXCEPT
	{
		return m_refcount.load( std::memory_order_relaxed );
	}
}

/*
R_EMIT( target = lua_beforemethods )
local function reference_add(self)
	return methodlist.utils_refobject_addref(self)
end
R_END()
*/

#endif
