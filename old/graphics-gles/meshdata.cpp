#include "meshdata.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <cstring>

namespace graphics
{
#define finally( op ) catch( ... ) { { op } throw; } { op }
	void MeshData::doadvance( int framecount )
	{
		if( !m_dirty.exchange( false, std::memory_order_relaxed ) )
		{
			return;
		}
		MeshBuffer* mb = 0;
		do
		{
			mb = trylockfront();
		}
			while( !mb );
		try
		{
			if( mb->m_vertexdata )
			{
				if( m_vertexbuffersize != mb->m_vertexdata->m_capacity )
				{
					m_vertexbuffersize = mb->m_vertexdata->m_capacity;
					if( !m_vertexbuffer )
					{
						glGenBuffers( 1, &m_vertexbuffer );
						checkerror();
					}
					glBindBuffer( GL_ARRAY_BUFFER, m_vertexbuffer );
					checkerror();
					glBufferData(
						GL_ARRAY_BUFFER,
						m_vertexbuffersize,
						0,
						GL_DYNAMIC_DRAW );
					checkerror();
				}
			}
			else
			{
				glDeleteBuffers( 1, &m_vertexbuffer );
				checkerror();
				m_vertexbuffer = 0;
			}
			if( mb->m_indexdata )
			{
				if( m_indexbuffersize != mb->m_indexdata->m_capacity )
				{
					m_indexbuffersize = mb->m_indexdata->m_capacity;
					if( !m_indexbuffer )
					{
						glGenBuffers( 1, &m_indexbuffer );
						checkerror();
					}
					glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer );
					checkerror();
					glBufferData(
						GL_ELEMENT_ARRAY_BUFFER,
						m_indexbuffersize,
						0,
						GL_DYNAMIC_DRAW );
					checkerror();
				}
			}
			else
			{
				glDeleteBuffers( 1, &m_indexbuffer );
				checkerror();
				m_indexbuffer = 0;
			}
			if( mb->m_vertexdata->m_length > 0 )
			{
				m_vertexcount = mb->m_vertexdata->m_length / sizeof( Vertex );
				glBindBuffer( GL_ARRAY_BUFFER, m_vertexbuffer );
				glBufferSubData(
					GL_ARRAY_BUFFER,
					0,
					mb->m_vertexdata->m_length,
					mb->m_vertexdata->m_data );
			}
			if( mb->m_indexdata->m_length > 0 )
			{
				m_indexcount = mb->m_indexdata->m_length / sizeof( uint16_t );
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer );
				glBufferSubData(
					GL_ELEMENT_ARRAY_BUFFER,
					0,
					mb->m_indexdata->m_length,
					mb->m_indexdata->m_data );
			}
		}
		finally(
			unlockfront( mb );
		)
	}

	MeshData::MeshData()
		: m_vertexbuffer( 0 )
		, m_indexbuffer( 0 )
		, m_vertexbuffersize( 0 )
		, m_indexbuffersize( 0 )
		, m_vertexcount( 0 )
		, m_indexcount( 0 )
		, m_frontbuffer( &m_buffer1 )
		, m_backbuffer( &m_buffer2 )
		, m_dirty( false )
	{
	}

	MeshData::~MeshData()
	{
		glDeleteBuffers( 1, &m_vertexbuffer );
		glDeleteBuffers( 1, &m_indexbuffer );
	}

	bool MeshData::bind( int* vertexcount, int* indexcount )
	{
		if( !m_vertexbuffer || !m_indexbuffer )
		{
			return false;
		}
		glBindBuffer( GL_ARRAY_BUFFER, m_vertexbuffer );
		checkerror();
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer );
		checkerror();
		glEnableVertexAttribArray( 0 );
		checkerror();
		glEnableVertexAttribArray( 1 );
		checkerror();
		glEnableVertexAttribArray( 2 );
		checkerror();
		glEnableVertexAttribArray( 3 );
		checkerror();
		glVertexAttribPointer(
			0, 3, GL_FLOAT, false, sizeof( Vertex ),
			( void* )offsetof( Vertex, pos_x ) );
		checkerror();
		glVertexAttribPointer(
			1, 2, GL_FLOAT, false, sizeof( Vertex ),
			( void* )offsetof( Vertex, tex1_x ) );
		checkerror();
		glVertexAttribPointer(
			2, 2, GL_FLOAT, false, sizeof( Vertex ),
			( void* )offsetof( Vertex, tex2_x ) );
		checkerror();
		glVertexAttribPointer(
			3, 4, GL_BYTE, true, sizeof( Vertex ),
			( void* )offsetof( Vertex, color ) );
		checkerror();
		*vertexcount = m_vertexcount;
		*indexcount = m_indexcount;
		return true;
	}

/*
	Threads:
		(g): graphics thread
		(l): logic thread
	Stable states:
		(1): valid front, valid back
		(2):  zero front, valid back; front held at (g)
		(3): valid front,  zero back; back held at (l) or at (g)
		(4):  zero front,  zero back; both held at (g) or at (g) and (l)
*/

	MeshBuffer* MeshData::trylockfront()
	{
		// (1) -> (2), returns front
		// (2) -> (2), returns 0
		// (3) -> (4), returns front
		// (4) -> (4), returns 0
		MeshBuffer* mb = m_frontbuffer.exchange( 0, std::memory_order_acquire );
		return mb;
	}

	void MeshData::unlockfront( MeshBuffer* mb )
	{
		// (1) invalid
		// (2) -> (1), returns 0
		// (3) -> (3), returns front (which means mb is now back)
		// (4) -> (3), returns 0
		MeshBuffer* oldmb = m_frontbuffer.exchange(
			mb, std::memory_order_relaxed );
		if( oldmb )
		{
			// (3) -> (1)
			m_frontbuffer.store( oldmb, std::memory_order_relaxed );
			m_backbuffer.store( mb, std::memory_order_release );
			// memory_order_release prevents (l) from seeing front == back
		}
	}

	MeshBuffer* MeshData::trylockback()
	{
		// (1) -> (3), returns back
		// (2) -> (4), returns back
		// (3) -> (3), returns 0
		// (4) -> (4), returns 0
		MeshBuffer* mb = m_backbuffer.exchange( 0, std::memory_order_relaxed );
		return mb;
	}

	void MeshData::unlockback( MeshBuffer* mb )
	{
		// (1) invalid
		// (2) invalid
		// (3) -> (3), returns front
		// (4) -> (3), returns 0
		MeshBuffer* oldmb = m_frontbuffer.exchange(
			mb, std::memory_order_release );
		if( oldmb )
		{
			// (3) -> (1)
			m_backbuffer.store( oldmb, std::memory_order_relaxed );
		}
		m_dirty.store( true, std::memory_order_relaxed );
	}

	MeshData* graphics_meshdata_new() noexcept
	{
	CBASE_PROTECT(
		return new MeshData();
	)
	}

	int graphics_meshdata_trylock( MeshData* md, MeshBuffer** pmb ) noexcept
	{
	CBASE_PROTECT(
		*pmb = md->trylockback();
		return *pmb ? 1 : 2;
	)
	}

	bool graphics_meshdata_unlock( MeshData* md, MeshBuffer* mb ) noexcept
	{
	CBASE_PROTECT(
		md->unlockback( mb );
		return 1;
	)
	}
}
