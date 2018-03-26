#include <rsbin/filestorage.hpp>
#include <utils/path.hpp>
#include <common/ref.hpp>
#include <stdexcept>
#include <exception>
#include <tuple>
#include <cstring>
#include <utils/console.hpp>

namespace rsbin
{
	enum
	{
		PageSize = 0x1000, // 4K
	};

	namespace
	{
		void trimlength( uint64_t offsetlimit, uint64_t offset, unsigned& length )
		{
			if( offset >= offsetlimit )
			{
				length = 0;
			}
			else
			{
				unsigned lengthlimit = offsetlimit - offset;
				if( length > lengthlimit )
				{
					length = lengthlimit;
				}
			}
		}

		void trimlength( unsigned offsetlimit, unsigned offset, unsigned& length )
		{
			if( offset >= offsetlimit )
			{
				length = 0;
			}
			else
			{
				unsigned lengthlimit = offsetlimit - offset;
				if( length > lengthlimit )
				{
					length = lengthlimit;
				}
			}
		}
	}

#if defined( _WIN32 ) || defined( _WIN64 )
	FileStorage::page_t::page_t( uint64_t offset )
	{
		m_refcount = 0;
		m_writercount = 0;
		m_isioactive = false;
		m_overlapped.Internal = 0;
		m_overlapped.InternalHigh = 0;
		m_overlapped.Offset = ( DWORD )( offset );
		m_overlapped.OffsetHigh = ( DWORD )( offset >> 32 );
		m_overlapped.hEvent = CreateEvent( nullptr, true, true, nullptr );
		if( !m_overlapped.hEvent )
		{
			syserror();
		}
		m_buffer = ( uint8_t* )VirtualAlloc(
			nullptr, PageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
		if( !m_buffer )
		{
			CloseHandle( m_overlapped.hEvent );
			syserror();
		}
	}

	FileStorage::page_t::~page_t()
	{
		CloseHandle( m_overlapped.hEvent );
		VirtualFree( m_buffer, 0, MEM_RELEASE );
	}

	FileStorage::page_t* FileStorage::getpage(
		uint64_t pageoffset, bool& isloaded )
	{
		auto it = m_pages.find( pageoffset );
		if( it != m_pages.end() )
		{
			isloaded = true;
			return &it->second;
		}
		clearpages();
		std::tie( it, std::ignore ) = m_pages.emplace(
			std::piecewise_construct,
			std::forward_as_tuple( pageoffset ),
			std::forward_as_tuple( pageoffset ) );
		isloaded = false;
		return &it->second;
	}

	void FileStorage::clearpages()
	{
		auto it = m_pages.begin();
		while( it != m_pages.end() )
		{
			page_t* page = &it->second;
			if( page->m_refcount == 0 && pollpage( page ) )
			{
				it = m_pages.erase( it );
			}
			else
			{
				++it;
			}
		}
	}

	bool FileStorage::pollpage( page_t* page )
	{
		if( !page->m_isioactive )
		{
			return true;
		}
		DWORD result;
		if( !GetOverlappedResult(
			m_handle, &page->m_overlapped, &result, false ) )
		{
			DWORD err = GetLastError();
			if( err == ERROR_IO_INCOMPLETE )
			{
				return false;
			}
			else
			{
				syserrorex( err );
			}
		}
		page->m_isioactive = false;
		return true;
	}

	bool FileStorage::startread( page_t* page )
	{
		ASSERT( !page->m_isioactive );
		if( !ReadFile(
			m_handle,
			page->m_buffer, PageSize,
			nullptr, &page->m_overlapped ) )
		{
			DWORD err = GetLastError();
			if( err == ERROR_HANDLE_EOF )
			{
				return true;
			}
			else if( err == ERROR_IO_PENDING )
			{
				page->m_isioactive = true;
				return false;
			}
			else
			{
				syserrorex( err );
			}
		}
		return true;
	}

	bool FileStorage::startwrite( page_t* page )
	{
		ASSERT( !page->m_isioactive );
		if( !WriteFile(
			m_handle,
			page->m_buffer, PageSize,
			nullptr, &page->m_overlapped ) )
		{
			DWORD err = GetLastError();
			if( err == ERROR_IO_PENDING )
			{
				page->m_isioactive = true;
				return false;
			}
			else
			{
				syserrorex( err );
			}
		}
		return true;
	}

	struct FileParameters
	{
		DWORD desiredaccess;
		DWORD sharemode;
		DWORD creationdisposition;
	};

	static FileParameters fp_map[] =
	{
		{
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			OPEN_EXISTING,
		},
		{
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			OPEN_EXISTING,
		},
		{
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			CREATE_ALWAYS,
		},
		{
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			OPEN_ALWAYS,
		},
	};

	FileStorage::FileStorage( char const* path, fileopenmode mode )
	{
		if( int( mode ) < 0 ||
			int( mode ) >= int( sizeof( fp_map ) / sizeof( fp_map[ 0 ] ) ) )
		{
			throw std::runtime_error( "invalid file mode" );
		}
		Ref< utils::Path > upath = utils::Path::create( path );
		if( !upath )
		{
			throw std::runtime_error( "invalid path" );
		}
		FileParameters* fp = &fp_map[ int( mode ) ];
		m_handle = CreateFileW(
			upath->combine(),
			fp->desiredaccess,
			fp->sharemode,
			nullptr,
			fp->creationdisposition,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
			0 );
		if( m_handle == INVALID_HANDLE_VALUE )
		{
			syserror();
		}
		LARGE_INTEGER filesize;
		if( !GetFileSizeEx( m_handle, &filesize ) )
		{
			CloseHandle( m_handle );
			syserror();
		}
		m_limit = filesize.QuadPart;
		m_mapcount = 0;
		m_busy = false;
		m_isreadable =
			mode == fileopenmode::read ||
			mode == fileopenmode::update ||
			mode == fileopenmode::provide;
		m_iswriteable =
			mode == fileopenmode::update ||
			mode == fileopenmode::create ||
			mode == fileopenmode::provide;
	}

	FileStorage::~FileStorage()
	{
		ASSERT( !m_busy && m_mapcount == 0 );
		if( m_handle != INVALID_HANDLE_VALUE )
		{
			{
			wait:
				for( auto& kv : m_pages )
				{
					page_t* page = &kv.second;
					if( !pollpage( page ) )
					{
						goto wait;
					}
				}
			}
			if( m_iswriteable )
			{
				SetFileInformationByHandle(
					m_handle, FileEndOfFileInfo,
					&m_limit, sizeof( m_limit ) );
			}
			CloseHandle( m_handle );
			m_handle = INVALID_HANDLE_VALUE;
		}
	}

	class FileStorageMap: public StorageMap
	{
	private:
		Ref< FileStorage > m_target;
		FileStorage::page_t* m_page;
		unsigned m_offset;
		unsigned m_length;
		bool m_flagwrite;

	public:
		FileStorageMap(
			FileStorage* target, FileStorage::page_t* page,
			unsigned offset, unsigned length,
			bool flagwrite );
		~FileStorageMap();
		virtual bool poll() override;
		virtual void getmap(
			uint8_t** pmapptr, unsigned* pmaplength ) override;
	};

	StorageMap* FileStorage::map(
		uint64_t offset, uint32_t length,
		bool flagread, bool flagwrite )
	{
		if( m_handle == INVALID_HANDLE_VALUE )
		{
			throw std::runtime_error( "invalid FileStorage object" );
		}
		if( m_busy )
		{
			throw std::runtime_error(
				"the object is already busy with another operation" );
		}
		if( ( !m_isreadable && flagread ) || ( !m_iswriteable && flagwrite ) )
		{
			throw std::runtime_error(
				"the object does not allow the requested operation" );
		}
		if( !flagwrite )
		{
			trimlength( m_limit, offset, length );
		}
		unsigned localoffset = offset % PageSize;
		uint64_t pageoffset = offset - localoffset;
		trimlength( PageSize, localoffset, length );
		if( length == 0 )
		{
			return nullptr;
		}
		if( flagwrite )
		{
			if( m_limit < offset + length )
			{
				m_limit = offset + length;
			}
		}
		bool isloaded;
		page_t* page = getpage( pageoffset, isloaded );
		if( !isloaded )
		{
			if( flagread || localoffset != 0 || length != PageSize )
			{
				startread( page );
			}
		}
		return new FileStorageMap(
			this, page, localoffset, length, flagwrite );
	}

	FileStorageMap::FileStorageMap(
		FileStorage* target, FileStorage::page_t* page,
		unsigned offset, unsigned length,
		bool flagwrite )
		: m_target( target )
		, m_page( page )
		, m_offset( offset )
		, m_length( length )
		, m_flagwrite( flagwrite )
	{
		m_target->m_mapcount += 1;
		m_page->m_refcount += 1;
		if( m_flagwrite )
		{
			m_page->m_writercount += 1;
		}
	}

	FileStorageMap::~FileStorageMap()
	{
		m_page->m_refcount -= 1;
		if( m_flagwrite )
		{
			m_page->m_writercount -= 1;
			if( m_page->m_writercount == 0 )
			{
				if( m_target->pollpage( m_page ) )
				{
					m_target->startwrite( m_page );
				}
			}
		}
		m_target->m_mapcount -= 1;
	}

	bool FileStorageMap::poll()
	{
		return m_target->pollpage( m_page );
	}

	void FileStorageMap::getmap(
		uint8_t** pmapptr, unsigned* pmaplength )
	{
		ASSERT( m_target->pollpage( m_page ) );
		if( m_length > 0 )
		{
			*pmapptr = m_page->m_buffer + m_offset;
			*pmaplength = m_length;
		}
		else
		{
			*pmapptr = nullptr;
			*pmaplength = 0;
		}
	}

	GetLimitTask* FileStorage::getlimit( uint64_t* plimit )
	{
		if( m_handle == INVALID_HANDLE_VALUE )
		{
			throw std::runtime_error( "invalid FileStorage object" );
		}
		if( m_busy )
		{
			throw std::runtime_error(
				"the object is already busy with another operation" );
		}
		*plimit = m_limit;
		return nullptr;
	}

	Task* FileStorage::setlimit( uint64_t limit )
	{
		if( m_handle == INVALID_HANDLE_VALUE )
		{
			throw std::runtime_error( "invalid FileStorage object" );
		}
		if( m_busy || m_mapcount != 0 )
		{
			throw std::runtime_error(
				"the object is already busy with another operation" );
		}
		if( !m_iswriteable )
		{
			throw std::runtime_error(
				"the object does not allow the requested operation" );
		}
		m_limit = limit;
		return nullptr;
	}

	class FileStorageFlushTask: public Task
	{
	private:
		Ref< FileStorage > m_target;
		bool m_flagclose;
		bool m_isdone;

	public:
		FileStorageFlushTask(
			FileStorage* target, bool flagclose );
		~FileStorageFlushTask();
		virtual bool poll() override;
	};

	Task* FileStorage::flush()
	{
		if( m_handle == INVALID_HANDLE_VALUE )
		{
			throw std::runtime_error( "invalid FileStorage object" );
		}
		if( m_busy || m_mapcount != 0 )
		{
			throw std::runtime_error(
				"the object is already busy with another operation" );
		}
		return new FileStorageFlushTask( this, false );
	}

	Task* FileStorage::close()
	{
		if( m_handle == INVALID_HANDLE_VALUE )
		{
			throw std::runtime_error( "invalid FileStorage object" );
		}
		if( m_busy || m_mapcount != 0 )
		{
			throw std::runtime_error(
				"the object is already busy with another operation" );
		}
		return new FileStorageFlushTask( this, true );
	}

	FileStorageFlushTask::FileStorageFlushTask(
		FileStorage* target, bool flagclose )
		: m_target( target )
		, m_flagclose( flagclose )
		, m_isdone( false )
	{
		m_target->m_busy = true;
	}

	FileStorageFlushTask::~FileStorageFlushTask()
	{
		if( !m_isdone )
		{
			m_target->m_busy = false;
		}
	}

	bool FileStorageFlushTask::poll()
	{
		if( !m_isdone )
		{
			m_target->clearpages();
			if( !m_target->m_pages.empty() )
			{
				return false;
			}
			if( m_target->m_iswriteable )
			{
				if( !SetFileInformationByHandle(
					m_target->m_handle, FileEndOfFileInfo,
					&m_target->m_limit, sizeof( m_target->m_limit ) ) )
				{
					syserror();
				}
			}
			if( m_flagclose )
			{
				CloseHandle( m_target->m_handle );
				m_target->m_handle = INVALID_HANDLE_VALUE;
			}
			else
			{
				if( m_target->m_iswriteable )
				{
					FlushFileBuffers( m_target->m_handle );
				}
			}
			m_isdone = true;
			m_target->m_busy = false;
		}
		return true;
	}

#elif defined( __ANDROID__ )

	FileStorage::page_t()
		: offset( ( uint64_t )-1 )
	{
		// NYI
	}

	FileStorage::~page_t()
	{
		// NYI
	}

	struct FileParameters
	{
		int flags;
		mode_t createmode;
	};

	static FileParameters fp_map[] = {
		{
			O_RDONLY,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
		},
		{
			O_RDWR,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
		},
		{
			O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
		},
		{
			O_RDWR | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
		},
		{
			O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
		},
	};

	FileStorage::FileStorage( char const* path, fileopenmode mode )
	{
		if( int( mode ) < 0 ||
			int( mode ) >= int( sizeof( fp_map ) / sizeof( fp_map[ 0 ] ) ) )
		{
			throw std::runtime_error( "invalid file mode" );
		}
		Ref< utils::Path > upath = utils::Path::create( path );
		if( !upath )
		{
			throw std::runtime_error( "invalid path" );
		}
		FileParameters* fp = &fp_map[ int( mode ) ];
		m_handle = open( upath->combine(), fp->flags, fp->createmode );
		if( m_handle == -1 )
		{
			syserror();
		}
	}

	FileStorage::~FileStorage()
	{
		close( m_handle );
	}
#endif
}
