#ifndef GRAPHICS_STATICINDEXBUFFER_HPP__
#define GRAPHICS_STATICINDEXBUFFER_HPP__ 1

#include "indexbuffer.hpp"
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>

namespace graphics
{
	class StaticIndexBuffer: public IndexBuffer
	{
	private:
		utils::Ref< utils::DataBuffer > m_data;

	protected:
		virtual void doadvance() override;

	public:
		StaticIndexBuffer( utils::DataBuffer* data );
		virtual ~StaticIndexBuffer() override;
		StaticIndexBuffer( StaticIndexBuffer const& ) = delete;
		StaticIndexBuffer& operator=( StaticIndexBuffer const& ) = delete;
	};

	StaticIndexBuffer* graphics_staticindexbuffer_new(
		utils::DataBuffer* data ) noexcept;
}

#endif
