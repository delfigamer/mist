#include <renderer-d3d9/vertexbuffer.hpp>
#include <renderer-d3d9/context.hpp>
#include <renderer-d3d9/common.hpp>

namespace graphics
{
	void VertexBuffer::doadvance()
	{
		Ref< VertexDeclaration > vd = m_vertexdeclaration.getref();
		if( vd )
		{
			vd->advance();
		}
	}

	VertexBuffer::VertexBuffer()
		: m_buffercapacity( 0 )
		, m_buffersize( 0 )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::bind( size_t* vertexcount, VertexDeclaration** pvd )
	{
		Ref< VertexDeclaration > vd = m_vertexdeclaration.getref();
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
