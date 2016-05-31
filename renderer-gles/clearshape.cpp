#include <renderer-gles/clearshape.hpp>
#include <renderer-gles/common.hpp>
#include <utils/cbase.hpp>

namespace graphics
{
	void ClearShape::color_t::get( float* data )
	{
		lock_t lock( m_mutex );
		memcpy( data, m_data, sizeof( m_data ) );
	}

	void ClearShape::color_t::set( float const* data )
	{
		lock_t lock( m_mutex );
		memcpy( m_data, data, sizeof( m_data ) );
	}

	void ClearShape::doadvance()
	{
	}

	ClearShape::ClearShape( bool ccolor, bool cdepth, bool cstencil )
		: m_flags(
			( ccolor ? GL_COLOR_BUFFER_BIT : 0 ) |
			( cdepth ? GL_DEPTH_BUFFER_BIT : 0 ) |
			( cstencil ? GL_STENCIL_BUFFER_BIT : 0 )
		)
	{
	}

	ClearShape::~ClearShape()
	{
	}

	void ClearShape::paint()
	{
		int flags = m_flags.load( std::memory_order_relaxed );
		if( flags & GL_COLOR_BUFFER_BIT )
		{
			float color[ 4 ];
			m_color.get( color );
			glClearColor( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
			checkerror();
		}
		if( flags & GL_DEPTH_BUFFER_BIT )
		{
			float depth = m_depth.load( std::memory_order_relaxed );
			glClearDepthf( depth );
			checkerror();
		}
		if( flags & GL_STENCIL_BUFFER_BIT )
		{
			uint32_t stencil = m_stencil.load( std::memory_order_relaxed );
			glClearStencil( stencil );
			checkerror();
		}
		if( flags )
		{
			glClear( flags );
			checkerror();
		}
	}

	void ClearShape::setcolor( float const* value )
	{
		m_color.set( value );
	}

	void ClearShape::setdepth( float value )
	{
		m_depth.store( value, std::memory_order_relaxed );
	}

	void ClearShape::setstencil( uint32_t value )
	{
		m_stencil.store( value, std::memory_order_relaxed );
	}
}
