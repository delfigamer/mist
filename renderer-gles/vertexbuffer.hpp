#pragma once

#include <renderer-gles/resource.hpp>
#include <renderer-gles/vertexdeclaration.hpp>
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
		unsigned int m_vertexbuffer;
		size_t m_buffercapacity;
		size_t m_buffersize;

		virtual void doadvance() override;

	public:
		VertexBuffer();
		~VertexBuffer();

		bool bind( size_t* vertexcount, VertexDeclaration** pvd );
	};
}
