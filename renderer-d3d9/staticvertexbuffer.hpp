#pragma once

#include <renderer-d3d9/vertexbuffer.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <atomic>

namespace graphics
{
	R_CLASS( name = staticvertexbuffer )
	class StaticVertexBuffer: public VertexBuffer
	{
	private:
		Ref< DataBuffer > m_data;

	protected:
		virtual void doadvance() override;

	public:
		StaticVertexBuffer( VertexDeclaration* vd );
		virtual ~StaticVertexBuffer() override;
		StaticVertexBuffer( StaticVertexBuffer const& ) = delete;
		StaticVertexBuffer& operator=( StaticVertexBuffer const& ) = delete;

		R_METHOD() static StaticVertexBuffer* create( VertexDeclaration* vd )
		{
			return new StaticVertexBuffer( vd );
		}

		R_METHOD() void assign( DataBuffer* data ) NOEXCEPT
		{
			std::atomic_thread_fence( std::memory_order_release );
			m_data = data;
		}
	};
}
