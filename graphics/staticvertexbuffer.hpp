#ifndef GRAPHICS_STATICVERTEXBUFFER_HPP__
#define GRAPHICS_STATICVERTEXBUFFER_HPP__ 1

#include "vertexbuffer.hpp"
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>

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
		StaticVertexBuffer( utils::DataBuffer* data );
		virtual ~StaticVertexBuffer() override;
		StaticVertexBuffer( StaticVertexBuffer const& ) = delete;
		StaticVertexBuffer& operator=( StaticVertexBuffer const& ) = delete;

		R_METHOD() static StaticVertexBuffer* create(
			utils::DataBuffer* data )
		{
			return new StaticVertexBuffer( data );
		}
	};
}

#endif
