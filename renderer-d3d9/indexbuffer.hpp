#pragma once

#include <renderer-d3d9/resource.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = indexbuffer )
	class IndexBuffer: public Resource
	{
	protected:
		IDirect3DIndexBuffer9* m_buffer;
		int m_indexcount;

	public:
		IndexBuffer();
		virtual ~IndexBuffer() override;
		IndexBuffer( IndexBuffer const& ) = delete;
		IndexBuffer& operator=( IndexBuffer const& ) = delete;

		bool bind( int* indexcount );
	};
}
