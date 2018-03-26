#include <rsbin/memorystorage.hpp>
#include <common/ref.hpp>
#include <osapi.hpp>
#include <stdexcept>
#include <exception>
#include <cstring>

namespace rsbin
{
	enum
	{
		PageSize = 0x10000, // 64K
	};

	MemoryStorage::page_t::page_t()
	{
		m_buffer = nullptr;
	}

	MemoryStorage::page_t::page_t( page_t&& other )
	{
		m_buffer = other.m_buffer;
		other.m_buffer = nullptr;
	}

	MemoryStorage::page_t::~page_t()
	{
		if( m_buffer )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			VirtualFree( m_buffer, 0, MEM_RELEASE );
#else
			delete[] m_buffer;
#endif
		}
	}

	MemoryStorage::page_t& MemoryStorage::page_t::operator=( page_t&& other )
	{
		if( m_buffer )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			VirtualFree( m_buffer, 0, MEM_RELEASE );
#else
			delete[] m_buffer;
#endif
		}
		m_buffer = other.m_buffer;
		other.m_buffer = nullptr;
		return *this;
	}

	uint8_t* MemoryStorage::page_t::get( bool flagwrite )
	{
		if( !m_buffer && flagwrite )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			m_buffer = ( uint8_t* )VirtualAlloc(
				nullptr, PageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			if( !m_buffer )
			{
				syserror();
			}
#else
			m_buffer = new uint8_t[ PageSize ];
#endif
		}
		return m_buffer;
	}

	MemoryStorage::MemoryStorage()
		: m_limit( 0 )
	{
	}

	MemoryStorage::~MemoryStorage()
	{
	}

	class MemoryStorageMap: public StorageMap
	{
	private:
		Ref< MemoryStorage > m_target;
		uint8_t* m_ptr;
		unsigned m_length;

	public:
		MemoryStorageMap(
			MemoryStorage* target, uint8_t* ptr, unsigned length );
		~MemoryStorageMap();
		virtual bool poll() override;
		virtual void getmap(
			uint8_t** pmapptr, unsigned* pmaplength ) override;
	};

	StorageMap* MemoryStorage::map(
		uint64_t offset, uint32_t length,
		bool flagread, bool flagwrite )
	{
		if( length == 0 || ( !flagread && !flagwrite ) )
		{
			return nullptr;
		}
		if( !flagwrite )
		{
			if( offset >= m_limit )
			{
				return nullptr;
			}
			else
			{
				if( length > m_limit - offset )
				{
					length = m_limit - offset;
				}
			}
		}
		size_t pageindex = offset / PageSize;
		unsigned localoffset = offset % PageSize;
		if( length > PageSize - localoffset )
		{
			length = PageSize - localoffset;
		}
		if( flagwrite )
		{
			if( offset + length > m_limit )
			{
				setlimit( offset + length );
			}
		}
		ASSERT( pageindex < m_pages.size() );
		page_t* page = &m_pages[ pageindex ];
		uint8_t* buffer = page->get( flagwrite );
		if( buffer )
		{
			return new MemoryStorageMap( this, buffer + localoffset, length );
		}
		else
		{
			return nullptr;
		}
	}

	MemoryStorageMap::MemoryStorageMap(
		MemoryStorage* target, uint8_t* ptr, unsigned length )
		: m_target( target )
		, m_ptr( ptr )
		, m_length( length )
	{
	}

	MemoryStorageMap::~MemoryStorageMap()
	{
	}

	bool MemoryStorageMap::poll()
	{
		return true;
	}

	void MemoryStorageMap::getmap(
		uint8_t** pmapptr, unsigned* pmaplength )
	{
		*pmapptr = m_ptr;
		*pmaplength = m_length;
	}

	GetLimitTask* MemoryStorage::getlimit( uint64_t* plimit )
	{
		*plimit = m_limit;
		return nullptr;
	}

	Task* MemoryStorage::setlimit( uint64_t limit )
	{
		m_limit = limit;
		size_t pagecount = ( limit + PageSize - 1 ) / PageSize;
		m_pages.resize( pagecount );
		return nullptr;
	}

	Task* MemoryStorage::flush()
	{
		return nullptr;
	}

	Task* MemoryStorage::close()
	{
		m_pages.clear();
		m_limit = 0;
		return nullptr;
	}
}
