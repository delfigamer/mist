#pragma once

#include <common.hpp>
#include <atomic>
#include <cstdio>
#include <cinttypes>

namespace utils
{
	R_CLASS( name = refobject )
	class RefObject
	{
	private:
		mutable std::atomic< ptrdiff_t > m_refcount;

	public:
		ptrdiff_t addref() const NOEXCEPT;
		ptrdiff_t release() const NOEXCEPT;
		ptrdiff_t refcount() const NOEXCEPT;
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

	inline ptrdiff_t RefObject::addref() const NOEXCEPT
	{
		ptrdiff_t rc = m_refcount.fetch_add( 1, std::memory_order_relaxed ) + 1;
		return rc;
	}

	inline ptrdiff_t RefObject::release() const NOEXCEPT
	{
		if( !this )
		{
			return -1;
		}
		ptrdiff_t rc = m_refcount.fetch_sub( 1, std::memory_order_relaxed ) - 1;
		if( rc == 0 )
		{
			const_cast< RefObject* >( this )->destroy();
		}
		return rc;
	}

	inline ptrdiff_t RefObject::refcount() const NOEXCEPT
	{
		return m_refcount.load( std::memory_order_relaxed );
	}

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
	if self ~= nil then
		methodlist.refobject_addref(self)
	end
end

function refobject:release()
	methodlist.refobject_release(self.ptr)
	self.ptr = nil
	return
end
R_END()
*/
