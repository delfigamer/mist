#pragma once

#include <renderer-gles/resource.hpp>
#include <renderer-gles/vertexdeclaration.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace graphics
{
	R_CLASS( name = vertexbuffer )
	class VertexBuffer: public Resource
	{
	protected:
		utils::Ref< VertexDeclaration > m_vertexdeclaration;
		unsigned int m_vertexbuffer;
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
