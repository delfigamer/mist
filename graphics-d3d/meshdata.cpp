#include "meshdata.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <cstring>

namespace graphics
{
#define finally( op ) catch( ... ) { { op } throw; } { op }

	void MeshData::doadvance( IDirect3DDevice9* device, int framecount )
	{
		if( !m_dirty.exchange( false, std::memory_order_relaxed ) )
		{
			return;
		}
		if( !m_vertexdeclaration )
		{
			static D3DVERTEXELEMENT9 const VDElements[] =
			{
				{ 0, offsetof( Vertex, pos_x ), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, offsetof( Vertex, tex1_x ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 0, offsetof( Vertex, tex2_x ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
				{ 0, offsetof( Vertex, color ), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				D3DDECL_END(),
			};
			checkerror( device->CreateVertexDeclaration(
				VDElements,
				&m_vertexdeclaration ) );
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
					RELEASE( m_vertexbuffer );
					checkerror( device->CreateVertexBuffer(
						m_vertexbuffersize,
						D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
						0,
						D3DPOOL_DEFAULT,
						&m_vertexbuffer,
						0 ) );
				}
			}
			else
			{
				RELEASE( m_vertexbuffer );
			}
			if( mb->m_indexdata )
			{
				if( m_indexbuffersize != mb->m_indexdata->m_capacity )
				{
					m_indexbuffersize = mb->m_indexdata->m_capacity;
					RELEASE( m_indexbuffer );
					checkerror( device->CreateIndexBuffer(
						m_indexbuffersize,
						D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
						D3DFMT_INDEX16,
						D3DPOOL_DEFAULT,
						&m_indexbuffer,
						0 ) );
				}
			}
			else
			{
				RELEASE( m_indexbuffer );
			}
			if( mb->m_vertexdata->m_length > 0 )
			{
				void* vertices;
				m_vertexcount = mb->m_vertexdata->m_length / sizeof( Vertex );
				checkerror( m_vertexbuffer->Lock( 0, 0, &vertices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
				try
				{
					memcpy( vertices, mb->m_vertexdata->m_data, mb->m_vertexdata->m_length );
				}
				finally(
					m_vertexbuffer->Unlock();
				)
			}
			if( mb->m_indexdata->m_length > 0 )
			{
				void* indices;
				m_indexcount = mb->m_indexdata->m_length / sizeof( uint16_t );
				checkerror( m_indexbuffer->Lock( 0, 0, &indices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK ) );
				try
				{
					memcpy( indices, mb->m_indexdata->m_data, mb->m_indexdata->m_length );
				}
				finally(
					m_indexbuffer->Unlock();
				)
			}
		}
		finally(
			unlockfront( mb );
		)
	}

	MeshData::MeshData()
		: m_vertexbuffer( 0 )
		, m_indexbuffer( 0 )
		, m_vertexdeclaration( 0 )
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
		RELEASE( m_vertexbuffer );
		RELEASE( m_indexbuffer );
		RELEASE( m_vertexdeclaration );
	}

	bool MeshData::bind( IDirect3DDevice9* device, int* vertexcount, int* indexcount )
	{
		if( !m_vertexdeclaration || !m_vertexbuffer || !m_indexbuffer )
		{
			return false;
		}
		checkerror( device->SetVertexDeclaration( m_vertexdeclaration ) );
		checkerror( device->SetStreamSource( 0, m_vertexbuffer, 0, sizeof( Vertex ) ) );
		checkerror( device->SetIndices( m_indexbuffer ) );
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
		MeshBuffer* oldmb = m_frontbuffer.exchange( mb, std::memory_order_relaxed );
		if( oldmb )
		{
			// (3) -> (1)
			m_frontbuffer.store( oldmb, std::memory_order_relaxed );
			m_backbuffer.store( mb, std::memory_order_release ); // so (l) won't see front == back
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
		MeshBuffer* oldmb = m_frontbuffer.exchange( mb, std::memory_order_release );
		if( oldmb )
		{
			// (3) -> (1)
			m_backbuffer.store( oldmb, std::memory_order_relaxed );
		}
		m_dirty.store( true, std::memory_order_relaxed );
	}

	extern "C" {
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
}
