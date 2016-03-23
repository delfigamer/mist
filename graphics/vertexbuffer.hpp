#ifndef GRAPHICS_VERTEXBUFFER_HPP__
#define GRAPHICS_VERTEXBUFFER_HPP__ 1

#include <graphics/resource.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = vertexbuffer )
	class VertexBuffer: public Resource
	{
	protected:
		IDirect3DVertexDeclaration9* m_vertexdeclaration;
		IDirect3DVertexBuffer9* m_buffer;
		int m_vertexcount;

		virtual void doadvance() override;

	public:
		VertexBuffer();
		virtual ~VertexBuffer() override;
		VertexBuffer( VertexBuffer const& ) = delete;
		VertexBuffer& operator=( VertexBuffer const& ) = delete;

		bool bind( int* vertexcount );

		R_METHOD() static VertexBuffer* create()
		{
			return new VertexBuffer();
		}
	};
}

#endif
