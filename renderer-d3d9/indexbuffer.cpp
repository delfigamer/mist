#include <renderer-d3d9/indexbuffer.hpp>
#include <renderer-d3d9/common.hpp>
#include <utils/cbase.hpp>

namespace graphics
{
	IndexBuffer::IndexBuffer()
		: m_buffer( 0 )
		, m_indexcount( 0 )
	{
	}

	IndexBuffer::~IndexBuffer()
	{
		RELEASE( m_buffer );
	}

	bool IndexBuffer::bind( int* indexcount )
	{
		if( !m_buffer )
		{
			return false;
		}
		checkerror( Context::Device->SetIndices( m_buffer ) );
		*indexcount = m_indexcount;
		return true;
	}
}
