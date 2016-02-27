#ifndef GRAPHICS_VERTEXBUFFER_HPP__
#define GRAPHICS_VERTEXBUFFER_HPP__ 1

#include "resource.hpp"

namespace graphics
{
	class VertexBuffer: public Resource
	{
	protected:
		IDirect3DVertexDeclaration9* m_vertexdeclaration;
		IDirect3DVertexBuffer9* m_buffer;
		int m_vertexcount;

	public:
		VertexBuffer();
		virtual ~VertexBuffer() override;
		VertexBuffer( VertexBuffer const& ) = delete;
		VertexBuffer& operator=( VertexBuffer const& ) = delete;

		bool bind( int* vertexcount );
	};
}

#endif
