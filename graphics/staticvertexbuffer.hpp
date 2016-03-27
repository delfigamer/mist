#ifndef GRAPHICS_STATICVERTEXBUFFER_HPP__
#define GRAPHICS_STATICVERTEXBUFFER_HPP__ 1

#include <graphics/vertexbuffer.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <atomic>

namespace graphics
{
	R_CLASS( name = staticvertexbuffer )
	class StaticVertexBuffer: public VertexBuffer
	{
	private:
		utils::Ref< utils::DataBuffer > m_data;

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

		R_METHOD() void assign( utils::DataBuffer* data ) NOEXCEPT
		{
			std::atomic_thread_fence( std::memory_order_release );
			m_data = data;
		}
	};
}

#endif
