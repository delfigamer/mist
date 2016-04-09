#include <graphics/vertexbuffer.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>

namespace graphics
{
	void VertexBuffer::doadvance()
	{
		VertexDeclaration* vd = m_vertexdeclaration;
		if( vd )
		{
			vd->advance();
		}
	}

	VertexBuffer::VertexBuffer()
		: m_vertexdeclaration( nullptr )
		, m_vertexbuffer( 0 )
		, m_buffercapacity( 0 )
		, m_buffersize( 0 )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		RELEASE( m_vertexbuffer );
	}

	bool VertexBuffer::bind( size_t* vertexcount, VertexDeclaration** pvd )
	{
		VertexDeclaration* vd = m_vertexdeclaration;
		if( !vd || !m_vertexbuffer )
		{
			return false;
		}
		size_t vertexsize;
		if( !vd->bind( &vertexsize ) )
		{
			return false;
		}
		*pvd = vd;
		if( Context::CurrentVertexBuffer != m_vertexbuffer )
		{
			checkerror( Context::Device->SetStreamSource(
				0, m_vertexbuffer, 0, UINT( vertexsize ) ) );
			Context::CurrentVertexBuffer = m_vertexbuffer;
		}
		*vertexcount = m_buffersize / vertexsize;
		return true;
	}
}
