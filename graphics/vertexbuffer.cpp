#include <graphics/vertexbuffer.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
#include <utils/console.hpp>
#include <utils/cbase.hpp>

namespace graphics
{
	namespace
	{
		struct Vertex
		{
			float pos_x;
			float pos_y;
			float pos_z;
			uint8_t color[ 4 ];
		};
		D3DVERTEXELEMENT9 const VDElements[] = {
			{ 0, offsetof( Vertex, pos_x ), D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, offsetof( Vertex, color ), D3DDECLTYPE_UBYTE4N,
				D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			D3DDECL_END(),
		};
		Vertex const Vertices[] = {
			{ -0.5, -0.5, 0, 0xff, 0xff, 0xff, 0xff },
			{ -0.5,  0.5, 0, 0xff, 0xff, 0xff, 0xff },
			{  0.5, -0.5, 0, 0xff, 0xff, 0xff, 0xff },
			{  0.5,  0.5, 0, 0xff, 0xff, 0xff, 0xff },
		};
	}

	void VertexBuffer::doadvance()
	{
		if( !m_vertexdeclaration )
		{
			checkerror( Context::Device->CreateVertexDeclaration(
				VDElements,
				&m_vertexdeclaration ) );
		}
		if( !m_buffer )
		{
			m_vertexcount = sizeof( Vertices ) / sizeof( Vertex );
			checkerror( Context::Device->CreateVertexBuffer(
				sizeof( Vertices ),
				D3DUSAGE_WRITEONLY,
				0,
				D3DPOOL_DEFAULT,
				&m_buffer,
				0 ) );
			void* locktarget;
			checkerror( m_buffer->Lock(
				0, 0, &locktarget, D3DLOCK_NOSYSLOCK ) );
			try
			{
				memcpy( locktarget, Vertices, sizeof( Vertices ) );
			}
			catch( ... )
			{
				m_buffer->Unlock();
				throw;
			}
			checkerror( m_buffer->Unlock() );
		}
	}

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
