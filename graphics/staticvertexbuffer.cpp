#include <graphics/staticvertexbuffer.hpp>
#include <graphics/common.hpp>
#include <utils/cbase.hpp>
#include <atomic>
#include <cstring>

namespace graphics
{
	void StaticVertexBuffer::doadvance()
	{
		if( !m_vertexdeclaration )
		{
			static D3DVERTEXELEMENT9 const VDElements[] =
			{
				{ 0, offsetof( Vertex, pos_x ), D3DDECLTYPE_FLOAT3,
					D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, offsetof( Vertex, tex1_x ), D3DDECLTYPE_FLOAT2,
					D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 0, offsetof( Vertex, tex2_x ), D3DDECLTYPE_FLOAT2,
					D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
				{ 0, offsetof( Vertex, color ), D3DDECLTYPE_UBYTE4N,
					D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				D3DDECL_END(),
			};
			checkerror( Context::Device->CreateVertexDeclaration(
				VDElements,
				&m_vertexdeclaration ) );
		}
		if( !m_buffer )
		{
			utils::Ref< utils::DataBuffer > data = std::move( m_data );
			std::atomic_thread_fence( std::memory_order_acquire );
			checkerror( Context::Device->CreateVertexBuffer(
				data->m_length,
				D3DUSAGE_WRITEONLY,
				0,
				D3DPOOL_DEFAULT,
				&m_buffer,
				0 ) );
			void* vertices;
			m_vertexcount = data->m_length / sizeof( Vertex );
			checkerror( m_buffer->Lock(
				0, 0, &vertices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
			try
			{
				memcpy( vertices, data->m_data, data->m_length );
			}
			catch( ... )
			{
				m_buffer->Unlock();
				throw;
			}
			checkerror( m_buffer->Unlock() );
		}
	}

	StaticVertexBuffer::StaticVertexBuffer( utils::DataBuffer* data )
	{
		if( !data )
		{
			throw std::runtime_error( "data buffer expected" );
		}
		std::atomic_thread_fence( std::memory_order_release );
		m_data = data;
	}

	StaticVertexBuffer::~StaticVertexBuffer()
	{
	}
}
