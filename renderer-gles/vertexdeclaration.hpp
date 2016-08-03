#pragma once

#include <renderer-gles/resource.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace graphics
{
	R_ENUM( name = vertexelementformat )
	enum class vertexelementformat
	{
		float1 = 0,
		float2 = 1,
		float3 = 2,
		float4 = 3,
		ubyte4 = 4,
		short2 = 5,
		short4 = 6,
		ubyte4n = 7,
		short2n = 8,
		short4n = 9,
		ushort2n = 10,
		ushort4n = 11,
		invalid = 12,
	};

/*
R_EMIT( target = lua_beforemetatypes )
	vertexdeclaration.format = vertexelementformat
R_END()
*/

	R_STRUCT( name = vertexdeclelement )
	struct VertexDeclElement
	{
		int attribute;
		int offset;
		int format;
	};

	R_CLASS( name = vertexdeclaration )
	class VertexDeclaration: public Resource
	{
	private:
		struct attriblayout_t
		{
			int index;
			int size;
			int type;
			bool normalized;
			void* offset;
		};

	private:
		utils::Ref< utils::DataBuffer > m_data;
		attriblayout_t m_layout[ 8 ];
		int m_attribcount;
		size_t m_vertexsize;

	protected:
		virtual void doadvance() override;

	public:
		VertexDeclaration( utils::DataBuffer* data, size_t vertexsize );
		virtual ~VertexDeclaration() override;
		VertexDeclaration( VertexDeclaration const& ) = delete;
		VertexDeclaration& operator=( VertexDeclaration const& ) = delete;

		bool bind( size_t* vertexsize );

		R_METHOD() static VertexDeclaration* create(
			utils::DataBuffer* data, size_t vertexsize )
		{
			return new VertexDeclaration( data, vertexsize );
		}
	};
}