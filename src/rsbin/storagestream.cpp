#include <rsbin/storagestream.hpp>

namespace rsbin
{
	void StorageStream::releasecurrentmap()
	{
		if( m_currentmaptask )
		{
			StorageMap map = m_currentmaptask->getmap();
			m_position += map.length;
			m_currentmaptask = nullptr;
		}
	}

	StorageStream::StorageStream(
		Storage* storage,
		bool flagread, bool flagwrite,
		uint64_t offset )
		: m_storage( storage )
		, m_flagread( flagread )
		, m_flagwrite( flagwrite )
		, m_position( offset )
	{
	}

	StorageStream::~StorageStream()
	{
	}

	Ref< MapTask > StorageStream::startadvance( uint32_t length )
	{
		releasecurrentmap();
		m_currentmaptask = m_storage->startmap(
			m_position, length, m_flagread, m_flagwrite );
		return m_currentmaptask;
	}

	void StorageStream::skip( uint32_t length )
	{
		releasecurrentmap();
		m_position += length;
	}

	uint64_t StorageStream::getposition()
	{
		return m_position;
	}
}
