#pragma once

#include <common.hpp>
#include <atomic>

class [[ r::class, r::name( "refobject" ) ]] RefObject
{
private:
	mutable std::atomic< ptrdiff_t > m_refcount;

protected:
	virtual ~RefObject() noexcept;
	virtual void destroy();

public:
	void AddRef() const noexcept;
	void Release() const noexcept;
	ptrdiff_t refcount() const noexcept;
	[[ r::method, r::hidden ]] void vaddref() const noexcept;
	[[ r::method, r::hidden ]] void vrelease() const noexcept;
	RefObject() noexcept;
	RefObject( RefObject const& other ) noexcept;
	RefObject( RefObject&& other ) noexcept;
	RefObject& operator=( RefObject const& other ) noexcept;
	RefObject& operator=( RefObject&& other ) noexcept;
};

inline void RefObject::AddRef() const noexcept
{
	m_refcount.fetch_add( 1, std::memory_order_relaxed );
}

inline void RefObject::Release() const noexcept
{
	if( m_refcount.fetch_sub( 1, std::memory_order_relaxed ) == 1 )
	{
		const_cast< RefObject* >( this )->destroy();
	}
}

inline ptrdiff_t RefObject::refcount() const noexcept
{
	return m_refcount.load( std::memory_order_relaxed );
}

inline void RefObject::vaddref() const noexcept
{
	AddRef();
}

inline void RefObject::vrelease() const noexcept
{
	Release();
}

inline RefObject::RefObject() noexcept
	: m_refcount( 1 )
{
}

inline RefObject::RefObject( RefObject const& other ) noexcept
	: m_refcount( 1 )
{
}

inline RefObject::RefObject( RefObject&& other ) noexcept
	: m_refcount( 1 )
{
}

inline RefObject::~RefObject() noexcept
{
}

inline void RefObject::destroy()
{
	delete this;
}

inline RefObject& RefObject::operator=( RefObject const& other ) noexcept
{
	return *this;
}

inline RefObject& RefObject::operator=( RefObject&& other ) noexcept
{
	return *this;
}

r_emit(<<
	function refobject:cast(other)
		if other and other.ptr ~= nil then
			r.refobject_vaddref(other.ptr)
			return self:new(other.ptr)
		else
			return nil
		end
	end

	function refobject:release()
		if self.ptr ~= nil then
			r.refobject_vrelease(self.ptr)
			self.ptr = nil
		end
	end
>>)
