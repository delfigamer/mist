#ifndef GRAPHICS_MESHBUFFER_HPP__
#define GRAPHICS_MESHBUFFER_HPP__ 1

// #include "resource.hpp"
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <atomic>

namespace graphics
{
	class MeshBuffer
	{
	public:
		utils::Ref< utils::DataBuffer > m_vertexdata;
		utils::Ref< utils::DataBuffer > m_indexdata;

	public:
		MeshBuffer();
		~MeshBuffer();
		MeshBuffer( MeshBuffer const& ) = delete;
		MeshBuffer( MeshBuffer&& ) = delete;
		MeshBuffer& operator=( MeshBuffer const& ) = delete;
		MeshBuffer& operator=( MeshBuffer&& ) = delete;

		utils::DataBuffer* getvertexdata();
		void setvertexdata( utils::DataBuffer* db );
		utils::DataBuffer* getindexdata();
		void setindexdata( utils::DataBuffer* db );
	};

	extern "C"
	{
		utils::DataBuffer* graphics_meshbuffer_getvertexdata( MeshBuffer* mb ) noexcept;
		bool graphics_meshbuffer_setvertexdata( MeshBuffer* mb, utils::DataBuffer* db ) noexcept;
		utils::DataBuffer* graphics_meshbuffer_getindexdata( MeshBuffer* mb ) noexcept;
		bool graphics_meshbuffer_setindexdata( MeshBuffer* mb, utils::DataBuffer* db ) noexcept;
	}
}

#endif
