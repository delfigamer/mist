#ifndef GRAPHICS_VERTEXBUFFER_HPP__
#define GRAPHICS_VERTEXBUFFER_HPP__ 1

#include <graphics/resource.hpp>
#include <graphics/vertexdeclaration.hpp>
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

#endif
