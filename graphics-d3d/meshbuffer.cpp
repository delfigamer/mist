#include "meshbuffer.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics
{
	MeshBuffer::MeshBuffer()
		: m_vertexdata( nullptr )
		, m_indexdata( nullptr )
	{
	}

	MeshBuffer::~MeshBuffer()
	{
	}

	utils::DataBuffer* MeshBuffer::getvertexdata()
	{
		return m_vertexdata;
	}

	void MeshBuffer::setvertexdata( utils::DataBuffer* db )
	{
		m_vertexdata = db;
	}

	utils::DataBuffer* MeshBuffer::getindexdata()
	{
		return m_indexdata;
	}

	void MeshBuffer::setindexdata( utils::DataBuffer* db )
	{
		m_indexdata = db;
	}

	extern "C"
	{
		utils::DataBuffer* graphics_meshbuffer_getvertexdata( MeshBuffer* mb ) noexcept
		{
		CBASE_PROTECT(
			return mb->getvertexdata();
		)
		}

		bool graphics_meshbuffer_setvertexdata( MeshBuffer* mb, utils::DataBuffer* db ) noexcept
		{
		CBASE_PROTECT(
			mb->setvertexdata( db );
			return 1;
		)
		}

		utils::DataBuffer* graphics_meshbuffer_getindexdata( MeshBuffer* mb ) noexcept
		{
		CBASE_PROTECT(
			return mb->getindexdata();
		)
		}

		bool graphics_meshbuffer_setindexdata( MeshBuffer* mb, utils::DataBuffer* db ) noexcept
		{
		CBASE_PROTECT(
			mb->setindexdata( db );
			return 1;
		)
		}
	}
}
