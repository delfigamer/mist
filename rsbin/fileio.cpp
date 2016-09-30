#include <rsbin/fileio.hpp>
#include <rsbin/fsthread.hpp>
#include <rsbin/iotask.hpp>
#include <common/ref.hpp>
#include <utils/path.hpp>
#include <osapi.hpp>
#include <thread>
#include <stdexcept>
#include <exception>
#include <cstring>

namespace rsbin
{
	enum
	{
		BlockSize = 0x1000, // 64K
		MemBlockSize = 0x1000, // 64K
		BlockingReadThreshold = 0x1000, // 4K
	};
	size_t const MaxViewSize = (
		sizeof( void* ) >= 8 ?
		0x10000000000ULL : // 1T
		0x1000000 ); // 16M

	enum class ioaction
	{
		read = 0,
		write = 1,
		truncate = 2,
	};

	class FileIoTask: public IoTask
	{
	public:
		Ref< FileIo > m_target;
		uint64_t m_offset;
		size_t m_length;
		uint8_t* m_buffer;
		ioaction m_action;

		FileIoTask();
		virtual ~FileIoTask();
		FileIoTask( FileIoTask const& ) = delete;
		FileIoTask& operator=( FileIoTask const& ) = delete;

		bool iterate() override;
	};

	FileIoTask::FileIoTask()
	{
	}

	FileIoTask::~FileIoTask()
	{
	}

	bool FileIoTask::iterate()
	{
		if( m_length <= 0 && m_action != ioaction::truncate )
		{
			return true;
		}
		try
		{
			uint64_t viewsize = m_target->m_viewsize;
			size_t result = 0;
			if( m_action == ioaction::read && m_offset < viewsize )
			{
				size_t length = m_length;
				if( length > MemBlockSize )
				{
					length = MemBlockSize;
				}
				if( m_offset + length > viewsize )
				{
					length = viewsize - m_offset;
				}
				memcpy(
					m_buffer,
					m_target->m_view + m_offset,
					length );
				result = length;
			}
			else
			{
#if defined( _WIN32 ) || defined( _WIN64 )
				HANDLE handle = ( HANDLE )m_target->m_handle;
				LARGE_INTEGER lioffset;
				lioffset.QuadPart = m_offset;
				if( !SetFilePointerEx( handle, lioffset, 0, FILE_BEGIN ) )
				{
					syserror();
				}
				size_t length = m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				DWORD dwresult = 0;
				switch( m_action )
				{
				case ioaction::read:
					if( !ReadFile(
						handle, m_buffer, DWORD( length ), &dwresult, 0 ) )
					{
						syserror();
					}
					break;

				case ioaction::write:
					if( !WriteFile(
						handle, m_buffer, DWORD( length ), &dwresult, 0 ) )
					{
						syserror();
					}
					break;

				case ioaction::truncate:
					if( !SetEndOfFile( handle ) )
					{
						syserror();
					}
					break;
				}
				result = dwresult;
#elif defined( __ANDROID__ )
				if( m_offset + m_length > 0x7fffffffULL )
				{
					throw std::runtime_error(
						"Android does not support files larger than 2GB" );
				}
				int handle = m_target->m_handle;
				if( lseek( handle, m_offset, SEEK_SET ) == -1 )
				{
					syserror();
				}
				size_t length = m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				switch( m_action )
				{
				case ioaction::read:
					result = read( handle, m_buffer, length );
					if( result == -1 )
					{
						syserror();
					}
					break;

				case ioaction::write:
					result = write( handle, m_buffer, length );
					if( result == -1 )
					{
						syserror();
					}
					break;

				case ioaction::truncate:
					if( ftruncate( handle, m_offset ) == -1 )
					{
						syserror();
					}
					break;
				}
#endif
			}
			m_result += result;
			m_offset += result;
			m_buffer += result;
			m_length -= result;
			return result == 0 || m_length == 0;
		}
		catch( std::exception const& error )
		{
			m_result = 0;
			m_error.setchars( error.what() );
			return true;
		}
		catch( ... )
		{
			m_result = 0;
			m_error.setchars( "unknown error" );
			return true;
		}
	}

#if defined( _WIN32 ) || defined( _WIN64 )
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
			GENERIC_WRITE,
			FILE_SHARE_READ,
			CREATE_ALWAYS,
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
#elif defined( __ANDROID__ )
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
#endif

	FileIo::FileIo( char const* path, fileopenmode mode )
		: m_handle( 0 )
	{
		if( int( mode ) < 0 ||
			int( mode ) >= int( sizeof( fp_map ) / sizeof( fp_map[ 0 ] ) ) )
		{
			throw std::runtime_error( "invalid file mode" );
		}
		Ref< utils::Path > upath = utils::Path::create( path );
		FileParameters* fp = &fp_map[ int( mode ) ];
#if defined( _WIN32 ) || defined( _WIN64 )
		m_handle = ( void* )CreateFileW(
			upath->combine(),
			fp->desiredaccess,
			fp->sharemode,
			0,
			fp->creationdisposition,
			FILE_ATTRIBUTE_NORMAL,
			0 );
		if( ( HANDLE )m_handle == INVALID_HANDLE_VALUE )
		{
			syserror();
		}
		if( mode == fileopenmode::read )
		{
			LARGE_INTEGER filesize;
			GetFileSizeEx( m_handle, &filesize );
			m_viewsize = filesize.QuadPart;
			if( m_viewsize > MaxViewSize )
			{
				m_viewsize = MaxViewSize;
			}
		}
		else
		{
			m_viewsize = 0;
		}
		if( m_viewsize != 0 )
		{
			m_mapping = ( void* )CreateFileMapping(
				( HANDLE )m_handle,
				0,
				PAGE_READONLY,
				DWORD( m_viewsize >> 32 ), DWORD( m_viewsize ),
				0 );
			if( m_mapping == 0 )
			{
				syserror();
			}
			m_view = ( uint8_t* )MapViewOfFile(
				m_mapping,
				FILE_MAP_READ,
				0, 0,
				size_t( m_viewsize ) );
		}
		else
		{
			m_mapping = 0;
			m_view = 0;
		}
#elif defined( __ANDROID__ )
		m_handle = open( upath->combine(), fp->flags, fp->createmode );
		if( m_handle == -1 )
		{
			syserror();
		}
		/*if( mode == fileopenmode::read )
		{
			struct stat filestat;
			if( fstat( m_handle, &filestat ) == -1 )
			{
				syserror();
			}
			m_viewsize = filestat.st_size;
			if( m_viewsize > MaxViewSize )
			{
				m_viewsize = MaxViewSize;
			}
		}
		else*/
		// Memory-mapping doesn't seem to work on Android
		{
			m_viewsize = 0;
		}
		if( m_viewsize != 0 )
		{
			m_view = ( uint8_t* )mmap( 0, m_viewsize, PROT_READ, 0, m_handle, 0 );
			if( m_view == ( uint8_t* )-1 )
			{
				syserror();
			}
		}
		else
		{
			m_view = 0;
		}
#endif
	}

	FileIo::~FileIo()
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		if( m_view )
		{
			UnmapViewOfFile( m_view );
		}
		if( m_mapping )
		{
			CloseHandle( ( HANDLE )m_mapping );
		}
		CloseHandle( m_handle );
#elif defined( __ANDROID__ )
		if( m_view )
		{
			munmap( m_view, m_viewsize );
		}
		close( m_handle );
#endif
	}

	IoTask* FileIo::startread(
		uint64_t offset, size_t length, void* buffer, bool promote )
	{
		FileIoTask* task = new FileIoTask;
		if( length < BlockingReadThreshold && offset + length <= m_viewsize )
		{
			memcpy(
				buffer,
				m_view + offset,
				length );
			task->m_result = length;
			task->m_finished.store( true, std::memory_order_release );
			return task;
		}
		task->m_target = this;
		task->m_offset = offset;
		task->m_length = length;
		task->m_buffer = ( uint8_t* )buffer;
		task->m_action = ioaction::read;
		if( promote )
		{
			FsThread->pushhigh( task );
		}
		else
		{
			FsThread->pushmain( task );
		}
		return task;
	}

	IoTask* FileIo::startwrite(
		uint64_t offset, size_t length, void const* buffer, bool promote )
	{
		FileIoTask* task = new FileIoTask;
		task->m_target = this;
		task->m_offset = offset;
		task->m_length = length;
		task->m_buffer = ( uint8_t* )buffer;
		task->m_action = ioaction::write;
		if( promote )
		{
			FsThread->pushhigh( task );
		}
		else
		{
			FsThread->pushmain( task );
		}
		return task;
	}

	void FileIo::setsize( uint64_t size )
	{
		FileIoTask* task = new FileIoTask;
		task->m_target = this;
		task->m_offset = size;
		task->m_length = 0;
		task->m_buffer = 0;
		task->m_action = ioaction::truncate;
		FsThread->pushhigh( task );
		while( !task->m_finished.load( std::memory_order_acquire ) )
		{
			std::this_thread::yield();
		}
		task->release();
	}
}
