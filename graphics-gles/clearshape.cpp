#include "clearshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics
{
	ClearShape::ClearShape()
		: m_color{ 0, 0, 0, 0 }
	{
	}

	ClearShape::~ClearShape()
	{
	}

	void ClearShape::paint()
	{
		float color[ 4 ];
		getcolor( color );
		glClearColor( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
		checkerror();
		glClear( GL_COLOR_BUFFER_BIT );
		checkerror();
	}

	void ClearShape::getcolor( float* color )
	{
		lock_t lock( m_colormutex );
		for( int i = 0; i < 4; i++ )
		{
			color[ i ] = m_color[ i ];
		}
	}

	void ClearShape::setcolor( float const* color )
	{
		lock_t lock( m_colormutex );
		for( int i = 0; i < 4; i++ )
		{
			m_color[ i ] = color[ i ];
		}
	}

	ClearShape* graphics_clearshape_new() noexcept
	{
	CBASE_PROTECT(
		return new ClearShape();
	)
	}

	bool graphics_clearshape_getcolor(
		ClearShape* shape, float* color ) noexcept
	{
	CBASE_PROTECT(
		shape->getcolor( color );
		return 1;
	)
	}

	bool graphics_clearshape_setcolor(
		ClearShape* shape, float const* color ) noexcept
	{
	CBASE_PROTECT(
		shape->setcolor( color );
		return 1;
	)
	}
}
