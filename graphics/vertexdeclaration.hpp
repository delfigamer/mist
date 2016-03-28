#ifndef GRAPHICS_VERTEXDECLARATION_HPP__
#define GRAPHICS_VERTEXDECLARATION_HPP__ 1

#include <graphics/resource.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>

namespace graphics
{
	R_ENUM( name = vertexelementformat )
	namespace vertexelementformat
	{
		enum
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
	}

/*
R_EMIT( target = lua_beforemetatypes )
	vertexdeclaration.format = vertexelementformat
R_END()
*/

	R_CLASS( name = vertexdeclelement )
	struct VertexDeclElement
	{
	R_STRUCT()
		int attribute;
		int offset;
		int format;
	R_END()
	};

	R_CLASS( name = vertexdeclaration )
	class VertexDeclaration: public Resource
	{
	private:
		IDirect3DVertexDeclaration9* m_vertexdeclaration;
		utils::Ref< utils::DataBuffer > m_data;
		int m_vertexsize;

	protected:
		virtual void doadvance() override;

	public:
		VertexDeclaration( utils::DataBuffer* data, int vertexsize );
		virtual ~VertexDeclaration() override;
		VertexDeclaration( VertexDeclaration const& ) = delete;
		VertexDeclaration& operator=( VertexDeclaration const& ) = delete;

		bool bind( int* vertexsize );

		R_METHOD() static VertexDeclaration* create(
			utils::DataBuffer* data, int vertexsize )
		{
			return new VertexDeclaration( data, vertexsize );
		}
	};
}

#endif
