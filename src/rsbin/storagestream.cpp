#include <rsbin/storagestream.hpp>

namespace rsbin
{
	void StorageStream::releasecurrentmap()
	{
		if( m_currentmap )
		{
			uint8_t* mapptr;
			unsigned maplength;
			m_currentmap->getmap( &mapptr, &maplength );
			m_position += maplength;
			m_currentmap = nullptr;
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

	StorageMap* StorageStream::advance( unsigned length )
	{
		releasecurrentmap();
		m_currentmap.possess( m_storage->map(
			m_position, length, m_flagread, m_flagwrite ) );
		return m_currentmap.share();
	}

	void StorageStream::skip( unsigned length )
	{
		releasecurrentmap();
		m_position += length;
	}

	uint64_t StorageStream::getposition()
	{
		return m_position;
	}
}
