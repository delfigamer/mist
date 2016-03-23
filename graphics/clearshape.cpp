#include <graphics/clearshape.hpp>
#include <graphics/common.hpp>
#include <utils/cbase.hpp>

namespace graphics
{
	void ClearShape::doadvance()
	{
	}

	ClearShape::ClearShape( bool ccolor, bool cdepth, bool cstencil )
		: m_flags(
			( ccolor ? D3DCLEAR_TARGET : 0 ) |
			( cdepth ? D3DCLEAR_ZBUFFER : 0 ) |
			( cstencil ? D3DCLEAR_STENCIL : 0 )
		)
	{
	}

	ClearShape::~ClearShape()
	{
	}

	void ClearShape::paint()
	{
		int flags = m_flags.load( std::memory_order_relaxed );
		uint32_t color = m_color.load( std::memory_order_relaxed );
		float depth = m_depth.load( std::memory_order_relaxed );
		int stencil = m_stencil.load( std::memory_order_relaxed );
		if( flags )
		{
			checkerror( Context::Device->Clear(
				0, 0, flags,
				color, depth, stencil ) );
		}
	}

	void ClearShape::setcolor( float const* value )
	{
		m_color.store( argb8( value ), std::memory_order_relaxed );
	}

	void ClearShape::setdepth( float value )
	{
		m_depth.store( value, std::memory_order_relaxed );
	}

	void ClearShape::setstencil( int value )
	{
		m_stencil.store( value, std::memory_order_relaxed );
	}
}
