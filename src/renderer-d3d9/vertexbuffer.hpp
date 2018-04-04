#pragma once

#include <renderer-d3d9/resource.hpp>
#include <renderer-d3d9/vertexdeclaration.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = vertexbuffer )
	class VertexBuffer: public Resource
	{
	protected:
		AtomicRef< VertexDeclaration > m_vertexdeclaration;
		Ref< IDirect3DVertexBuffer9 > m_vertexbuffer;
		size_t m_buffercapacity;
		size_t m_buffersize;

		virtual void doadvance() override;

	public:
		VertexBuffer();
		~VertexBuffer();
		VertexBuffer( VertexBuffer const& ) = delete;
		VertexBuffer& operator=( VertexBuffer const& ) = delete;

		bool bind( size_t* vertexcount, VertexDeclaration** pvd );
	};
}
