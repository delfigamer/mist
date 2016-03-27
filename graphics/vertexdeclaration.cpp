#include <graphics/vertexbuffer.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
#include <atomic>

namespace graphics
{
	namespace
	{
		void validatedecldata( int length, void* data )
		{
			bool attrused[ Limits::AttributeCount ] = { 0 };
			VertexDeclElement* elems = ( VertexDeclElement* )data;
			for( int i = 0; i < int( length / sizeof( VertexDeclElement ) ); ++i )
			{
				if( elems[ i ].attribute < 0 ||
					elems[ i ].attribute >= Limits::AttributeCount )
				{
					throw utils::StrException(
						"invalid attribute index for element %i", i );
				}
				if( elems[ i ].format < 0 ||
					elems[ i ].format >=
						VertexElementFormat::invalid )
				{
					throw utils::StrException(
						"invalid format for element %i", i );
				}
				if( attrused[ elems[ i ].attribute ] )
				{
					throw utils::StrException(
						"duplicate attribute indices at element %i", i );
				}
				attrused[ elems[ i ].attribute ] = true;
			}
		}

		int const elemtypetable[] = {
			D3DDECLTYPE_FLOAT1,
			D3DDECLTYPE_FLOAT2,
			D3DDECLTYPE_FLOAT3,
			D3DDECLTYPE_FLOAT4,
			D3DDECLTYPE_UBYTE4,
			D3DDECLTYPE_SHORT2,
			D3DDECLTYPE_SHORT4,
			D3DDECLTYPE_UBYTE4N,
			D3DDECLTYPE_SHORT2N,
			D3DDECLTYPE_SHORT4N,
			D3DDECLTYPE_USHORT2N,
			D3DDECLTYPE_USHORT4N,
		};
	}

	void VertexDeclaration::doadvance()
	{
		if( !m_vertexdeclaration )
		{
			utils::DataBuffer* data = m_data;
			std::atomic_thread_fence( std::memory_order_acquire );
			VertexDeclElement* elems = ( VertexDeclElement* )data->m_data;
			int elemcount = data->m_length / sizeof( VertexDeclElement );
			D3DVERTEXELEMENT9 d3delems[ 9 ];
			for( int i = 0; i < elemcount; ++i )
			{
				d3delems[ i ].Stream = 0;
				d3delems[ i ].Offset = elems[ i ].offset;
				d3delems[ i ].Type = elemtypetable[ elems[ i ].format ];
				d3delems[ i ].Method = D3DDECLMETHOD_DEFAULT;
				d3delems[ i ].Usage =
					AttributeUsageTable[ elems[ i ].attribute ][ 0 ];
				d3delems[ i ].UsageIndex =
					AttributeUsageTable[ elems[ i ].attribute ][ 1 ];
			}
			d3delems[ elemcount ] = D3DDECL_END();
			checkerror( Context::Device->CreateVertexDeclaration(
				d3delems,
				&m_vertexdeclaration ) );
		}
	}

	VertexDeclaration::VertexDeclaration(
		utils::DataBuffer* data, int vertexsize )
		: m_vertexdeclaration( 0 )
		, m_vertexsize( vertexsize )
	{
		validatedecldata( data->m_length, data->m_data );
		std::atomic_thread_fence( std::memory_order_release );
		m_data = data;
	}

	VertexDeclaration::~VertexDeclaration()
	{
		RELEASE( m_vertexdeclaration );
	}

	bool VertexDeclaration::bind( int* vertexsize )
	{
		if( !m_vertexdeclaration )
		{
			return false;
		}
		if( Context::CurrentVertexDeclaration != m_vertexdeclaration )
		{
			checkerror( Context::Device->SetVertexDeclaration(
				m_vertexdeclaration ) );
			Context::CurrentVertexDeclaration = m_vertexdeclaration;
		}
		*vertexsize = m_vertexsize;
		return true;
	}
}
