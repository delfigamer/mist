#pragma once

#include <common.hpp>
#include <atomic>
#include <cstdio>

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

inline RefObject::RefObject() noexcept
	: m_refcount( 1 )
{
	Counter< RefObject >::update( +1 );
}

inline RefObject::RefObject( RefObject const& other ) noexcept
	: m_refcount( 1 )
{
	Counter< RefObject >::update( +1 );
}

inline RefObject::RefObject( RefObject&& other ) noexcept
	: m_refcount( 1 )
{
	Counter< RefObject >::update( +1 );
}

inline RefObject::~RefObject() noexcept
{
	Counter< RefObject >::update( -1 );
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
