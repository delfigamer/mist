#include <renderer-gles/staticvertexbuffer.hpp>
#include <renderer-gles/common.hpp>
#include <renderer-gles/gl2.hpp>
#include <cstring>

namespace graphics
{
	void StaticVertexBuffer::doadvance()
	{
		utils::Ref< utils::DataBuffer > data = std::move( m_data );
		if( data )
		{
			if( !m_vertexbuffer )
			{
				glGenBuffers( 1, &m_vertexbuffer );
				checkerror();
			}
			if( Context::CurrentVertexBuffer != this )
			{
				glBindBuffer( GL_ARRAY_BUFFER, m_vertexbuffer );
				checkerror();
				Context::CurrentVertexBuffer = this;
			}
			std::atomic_thread_fence( std::memory_order_acquire );
			if( !m_vertexbuffer || m_buffercapacity != data->m_capacity )
			{
				m_buffercapacity = data->m_capacity;
				glBufferData(
					GL_ARRAY_BUFFER, m_buffercapacity, 0, GL_STATIC_DRAW );
				checkerror();
			}
			m_buffersize = data->m_length;
			glBufferSubData( GL_ARRAY_BUFFER, 0, m_buffersize, data->m_data );
			checkerror();
		}
		VertexBuffer::doadvance();
	}

	StaticVertexBuffer::StaticVertexBuffer( VertexDeclaration* vd )
		: m_data( nullptr )
	{
		m_vertexdeclaration = vd;
	}

	StaticVertexBuffer::~StaticVertexBuffer()
	{
	}
}
