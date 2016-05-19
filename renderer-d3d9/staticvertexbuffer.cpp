#include <renderer-d3d9/staticvertexbuffer.hpp>
#include <renderer-d3d9/common.hpp>
#include <utils/cbase.hpp>
#include <cstring>

namespace graphics
{
	void StaticVertexBuffer::doadvance()
	{
		utils::Ref< utils::DataBuffer > data = std::move( m_data );
		if( data )
		{
			std::atomic_thread_fence( std::memory_order_acquire );
			if( !m_vertexbuffer || m_buffercapacity != data->m_capacity )
			{
				RELEASE( m_vertexbuffer );
				m_buffercapacity = data->m_capacity;
				checkerror( Context::Device->CreateVertexBuffer(
					UINT( m_buffercapacity ),
					D3DUSAGE_WRITEONLY,
					0,
					D3DPOOL_DEFAULT,
					&m_vertexbuffer,
					0 ) );
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
		: m_data( nullptr )
	{
		m_vertexdeclaration = vd;
	}

	StaticVertexBuffer::~StaticVertexBuffer()
	{
	}
}
