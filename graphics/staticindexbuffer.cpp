#include <graphics/staticindexbuffer.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
#include <utils/cbase.hpp>
#include <cstring>

namespace graphics
{
	void StaticIndexBuffer::doadvance()
	{
		if( !m_buffer )
		{
			utils::Ref< utils::DataBuffer > data = std::move( m_data );
			std::atomic_thread_fence( std::memory_order_acquire );
			checkerror( Context::Device->CreateIndexBuffer(
				data->m_length,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_DEFAULT,
				&m_buffer,
				0 ) );
			void* indices;
			m_indexcount = data->m_length / sizeof( uint16_t );
			checkerror( m_buffer->Lock(
				0, 0, &indices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
			try
			{
				memcpy( indices, data->m_data, data->m_length );
			}
			catch( ... )
			{
				m_buffer->Unlock();
				throw;
			}
			checkerror( m_buffer->Unlock() );
		}
	}

	StaticIndexBuffer::StaticIndexBuffer( utils::DataBuffer* data )
	{
		if( !data )
		{
			throw std::runtime_error( "data buffer expected" );
		}
		std::atomic_thread_fence( std::memory_order_release );
		m_data = data;
	}

	StaticIndexBuffer::~StaticIndexBuffer()
	{
	}
}
