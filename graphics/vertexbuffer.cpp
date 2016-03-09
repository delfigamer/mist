#include <graphics/vertexbuffer.hpp>
#include <graphics/common.hpp>
#include <utils/cbase.hpp>

namespace graphics
{
	VertexBuffer::VertexBuffer()
		: m_vertexdeclaration( 0 )
		, m_buffer( 0 )
		, m_vertexcount( 0 )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		RELEASE( m_vertexdeclaration );
		RELEASE( m_buffer );
	}

	bool VertexBuffer::bind( int* vertexcount )
	{
		if( !m_vertexdeclaration || !m_buffer )
		{
			return false;
		}
		checkerror( Context::Device->SetVertexDeclaration(
			m_vertexdeclaration ) );
		checkerror( Context::Device->SetStreamSource(
			0, m_buffer, 0, sizeof( Vertex ) ) );
		*vertexcount = m_vertexcount;
		return true;
	}
}
