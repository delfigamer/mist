#include <renderer-d3d9/staticvertexbuffer.hpp>
#include <renderer-d3d9/common.hpp>
#include <cstring>

namespace graphics
{
	void StaticVertexBuffer::doadvance()
	{
		Ref< DataBuffer > data = m_data.detachref();
		if( data )
		{
			if( !m_vertexbuffer || m_buffercapacity != data->m_capacity )
			{
				IDirect3DVertexBuffer9* vertexbuffer = 0;
				m_buffercapacity = data->m_capacity;
				checkerror( Context::Device->CreateVertexBuffer(
					UINT( m_buffercapacity ),
					D3DUSAGE_WRITEONLY,
					0,
					D3DPOOL_DEFAULT,
					&vertexbuffer,
					0 ) );
				m_vertexbuffer.possess( vertexbuffer );
			}
			m_buffersize = data->m_length;
			void* vertices;
			checkerror( m_vertexbuffer->Lock(
				0, 0, &vertices, D3DLOCK_NOSYSLOCK ) );
			try
			{
				memcpy( vertices, data->m_data, data->m_length );
			}
			catch( ... )
			{
				m_vertexbuffer->Unlock();
				throw;
			}
			checkerror( m_vertexbuffer->Unlock() );
		}
		VertexBuffer::doadvance();
	}

	StaticVertexBuffer::StaticVertexBuffer( VertexDeclaration* vd )
	{
		m_vertexdeclaration.assign( vd );
	}

	StaticVertexBuffer::~StaticVertexBuffer()
	{
	}
}
