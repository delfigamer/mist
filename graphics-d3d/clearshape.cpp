#include "clearshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics
{
	void ClearShape::doadvance()
	{
	}

	ClearShape::ClearShape()
		: m_flags( 0 )
	{
	}

	ClearShape::~ClearShape()
	{
	}

	void ClearShape::paint()
	{
		int flags;
		uint32_t color;
		float depth;
		int stencil;
		{
			lock_t lock( m_mutex );
			flags = m_flags;
			color = m_color;
			depth = m_depth;
			stencil = m_stencil;
		}
		if( flags )
		{
			checkerror( Context::Device->Clear(
				0, 0, flags,
				color, depth, stencil ) );
		}
	}

	void ClearShape::setcolor( bool flag, float const* value )
	{
		lock_t lock( m_mutex );
		if( flag )
		{
			m_flags |= D3DCLEAR_TARGET;
			m_color = argb8( value );
		}
		else
		{
			m_flags &= ~D3DCLEAR_TARGET;
		}
	}

	void ClearShape::setdepth( bool flag, float value )
	{
		lock_t lock( m_mutex );
		if( flag )
		{
			m_flags |= D3DCLEAR_ZBUFFER;
			m_depth = value;
		}
		else
		{
			m_flags &= ~D3DCLEAR_ZBUFFER;
		}
	}

	void ClearShape::setstencil( bool flag, int value )
	{
		lock_t lock( m_mutex );
		if( flag )
		{
			m_flags |= D3DCLEAR_STENCIL;
			m_stencil = value;
		}
		else
		{
			m_flags &= ~D3DCLEAR_STENCIL;
		}
	}

	ClearShape* graphics_clearshape_new() noexcept
	{
	CBASE_PROTECT(
		return new ClearShape();
	)
	}

	bool graphics_clearshape_setcolor(
		ClearShape* shape, bool flag, float const* value ) noexcept
	{
	CBASE_PROTECT(
		shape->setcolor( flag, value );
		return 1;
	)
	}

	bool graphics_clearshape_setdepth(
		ClearShape* shape, bool flag, float value ) noexcept
	{
	CBASE_PROTECT(
		shape->setdepth( flag, value );
		return 1;
	)
	}

	bool graphics_clearshape_setstencil(
		ClearShape* shape, bool flag, int value ) noexcept
	{
	CBASE_PROTECT(
		shape->setstencil( flag, value );
		return 1;
	)
	}
}
