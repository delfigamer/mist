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
		, m_mapcount( 0 )
	{
	}

	MemoryStorage::~MemoryStorage()
	{
	}

	class MemoryStorageMapTask: public MapTask
	{
	private:
		Ref< MemoryStorage > m_target;
		uint8_t* m_ptr;
		uint32_t m_length;

	public:
		MemoryStorageMapTask(
			MemoryStorage* target, uint8_t* ptr, uint32_t length );
		~MemoryStorageMapTask();
		virtual bool poll() override;
		virtual StorageMap getmap() override;
	};

	Ref< MapTask > MemoryStorage::startmap(
		uint64_t offset, uint32_t length,
		bool flagread, bool flagwrite )
	{
		lock_t lock( m_mutex );
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
		uint32_t localoffset = offset % PageSize;
		if( length > PageSize - localoffset )
		{
			length = PageSize - localoffset;
		}
		if( flagwrite )
		{
			if( offset + length > m_limit )
			{
				startsetlimit( offset + length );
			}
		}
		assert( pageindex < m_pages.size() );
		page_t* page = &m_pages[ pageindex ];
		uint8_t* buffer = page->get( flagwrite );
		if( buffer )
		{
			return Ref< MemoryStorageMapTask >::create(
				this, buffer + localoffset, length );
		}
		else
		{
			return nullptr;
		}
	}

	MemoryStorageMapTask::MemoryStorageMapTask(
		MemoryStorage* target, uint8_t* ptr, uint32_t length )
		: m_target( target )
		, m_ptr( ptr )
		, m_length( length )
	{
		m_target->m_mapcount += 1;
	}

	MemoryStorageMapTask::~MemoryStorageMapTask()
	{
		MemoryStorage::lock_t lock( m_target->m_mutex );
		m_target->m_mapcount -= 1;
	}

	bool MemoryStorageMapTask::poll()
	{
		return true;
	}

	StorageMap MemoryStorageMapTask::getmap()
	{
		return StorageMap{ m_ptr, m_length };
	}

	Ref< GetLimitTask > MemoryStorage::startgetlimit( uint64_t* plimit )
	{
		lock_t lock( m_mutex );
		*plimit = m_limit;
		return nullptr;
	}

	Ref< Task > MemoryStorage::startsetlimit( uint64_t limit )
	{
		lock_t lock( m_mutex );
		if( m_mapcount != 0 )
		{
			throw std::runtime_error(
				"the object has currently active mappings" );
		}
		m_limit = limit;
		size_t pagecount = ( limit + PageSize - 1 ) / PageSize;
		m_pages.resize( pagecount );
		return nullptr;
	}

	Ref< Task > MemoryStorage::startflush()
	{
		if( m_mapcount != 0 )
		{
			throw std::runtime_error(
				"the object has currently active mappings" );
		}
		return nullptr;
	}

	Ref< Task > MemoryStorage::startclose()
	{
		lock_t lock( m_mutex );
		if( m_mapcount != 0 )
		{
			throw std::runtime_error(
				"the object has currently active mappings" );
		}
		m_pages.clear();
		m_limit = 0;
		return nullptr;
	}
}
