#ifndef UTILS_REFOBJECT_HPP__
#define UTILS_REFOBJECT_HPP__ 1

#include <common.hpp>
#include <cstdio>
#include <atomic>
#include <utils/console.hpp>

namespace utils
{
	R_CLASS( name = refobject )
	class RefObject
	{
	private:
		mutable std::atomic< int > m_refcount;

	public:
		int addref() const NOEXCEPT;
		int release() const NOEXCEPT;
		int refcount() const NOEXCEPT;
		R_METHOD( name = addref, noluamethod ) void vaddref() const NOEXCEPT
		{
			addref();
		}
		R_METHOD( name = release, noluamethod ) void vrelease() const NOEXCEPT
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
		return rc;
	}

	inline int RefObject::release() const NOEXCEPT
	{
		if( !this )
		{
			return -1;
		}
		int rc = m_refcount.fetch_sub( 1, std::memory_order_relaxed ) - 1;
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

	template< int i >
	struct rocount
	{
		static std::atomic< int > d;
	};

	template< int i >
	std::atomic< int > rocount< i >::d( 0 );

	inline RefObject::RefObject() NOEXCEPT
		: m_refcount( 1 )
	{
	}

	inline RefObject::RefObject( RefObject const& other ) NOEXCEPT
		: m_refcount( 1 )
	{
	}

	inline RefObject::RefObject( RefObject&& other ) NOEXCEPT
		: m_refcount( 1 )
	{
	}

	inline RefObject::~RefObject() NOEXCEPT
	{
	}

	inline void RefObject::destroy()
	{
		delete this;
	}

	inline RefObject& RefObject::operator=( RefObject const& other ) NOEXCEPT
	{
		return *this;
	}

	inline RefObject& RefObject::operator=( RefObject&& other ) NOEXCEPT
	{
		return *this;
	}
}

/*
R_EMIT( target = lua_beforemethods )
local function reference_addref(self)
	return methodlist.refobject_addref(self)
end

function refobject:release()
	methodlist.refobject_release(self.ptr)
	self.ptr = nil
	return
end
R_END()
*/

#endif
