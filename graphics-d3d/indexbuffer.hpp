#ifndef GRAPHICS_INDEXBUFFER_HPP__
#define GRAPHICS_INDEXBUFFER_HPP__ 1

#include "resource.hpp"

namespace graphics
{
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

#endif
