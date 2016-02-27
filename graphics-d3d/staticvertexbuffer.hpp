#ifndef GRAPHICS_STATICVERTEXBUFFER_HPP__
#define GRAPHICS_STATICVERTEXBUFFER_HPP__ 1

#include "vertexbuffer.hpp"
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>

namespace graphics
{
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
	};

	StaticVertexBuffer* graphics_staticvertexbuffer_new(
		utils::DataBuffer data ) noexcept;
}

#endif
