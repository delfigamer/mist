#pragma once

#include <renderer-d3d9/resource.hpp>
#include <renderer-d3d9/vertexdeclaration.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace graphics
{
	R_CLASS( name = vertexbuffer )
	class VertexBuffer: public Resource
	{
	protected:
		Ref< VertexDeclaration > m_vertexdeclaration;
		IDirect3DVertexBuffer9* m_vertexbuffer;
		size_t m_buffercapacity;
		size_t m_buffersize;

		virtual void doadvance() override;

	public:
		VertexBuffer();
		virtual ~VertexBuffer() override;
		VertexBuffer( VertexBuffer const& ) = delete;
		VertexBuffer& operator=( VertexBuffer const& ) = delete;

		bool bind( size_t* vertexcount, VertexDeclaration** pvd );
	};
}
