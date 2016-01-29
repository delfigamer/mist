#include "fileio.hpp"
#include "fsthread.hpp"
#include <utils/ref.hpp>
#include <utils/path.hpp>
#include <utils/cbase.hpp>
#include <osapi.hpp>
#include <thread>
#include <stdexcept>
#include <exception>

namespace rsbin
{
	int const BlockingReadThreshold = 0x1000; // 4K
	uint64_t const MaxViewSize = (
		sizeof( void* ) >= 8 ?
		0x10000000000ULL : // 1T
		0x1000000 ); // 16M

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
	FileIo::FileIo( char const* path, int mode )
		: m_handle( 0 )
	{
		if( mode < 0 || mode >= int( sizeof( fp_map ) / sizeof( fp_map[ 0 ] ) ) )
		{
			throw std::runtime_error( "invalid file mode" );
		}
		utils::Ref< utils::Path > upath = utils::Path::create( path );
		FileParameters* fp = &fp_map[ mode ];
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
		if( mode == FileMode_Read )
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
				m_viewsize >> 32, m_viewsize,
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
		/*if( mode == FileMode_Read )
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

	FsTask* FileIo::startread( uint64_t offset, int length, void* buffer )
	{
		FsTask* task = new FsTask;
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
		task->m_direction = IoActionRead;
		task->m_result = 0;
		FsThread->pushmain( task );
		return task;
	}

	FsTask* FileIo::startwrite( uint64_t offset, int length, void const* buffer )
	{
		FsTask* task = new FsTask;
		task->m_target = this;
		task->m_offset = offset;
		task->m_length = length;
		task->m_buffer = ( uint8_t* )buffer;
		task->m_direction = IoActionWrite;
		task->m_result = 0;
		FsThread->pushmain( task );
		return task;
	}

	void FileIo::setsize( uint64_t size )
	{
		FsTask* task = new FsTask;
		task->m_target = this;
		task->m_offset = size;
		task->m_length = 0;
		task->m_buffer = 0;
		task->m_direction = IoActionTruncate;
		task->m_result = 0;
		FsThread->pushhigh( task );
		while( !task->m_finished.load( std::memory_order_acquire ) )
		{
			std::this_thread::yield();
		}
		task->release();
	}

	FileIo* rsbin_fileio_new( char const* path, int mode ) noexcept
	{
	CBASE_PROTECT(
		return new FileIo( path, mode );
	)
	}

	FsTask* rsbin_fileio_startread(
		FileIo* io, uint64_t offset, int length, void* buffer ) noexcept
	{
	CBASE_PROTECT(
		return io->startread( offset, length, buffer );
	)
	}

	FsTask* rsbin_fileio_startwrite(
		FileIo* io, uint64_t offset, int length, void const* buffer ) noexcept
	{
	CBASE_PROTECT(
		return io->startwrite( offset, length, buffer );
	)
	}

	bool rsbin_fileio_setsize( FileIo* io, uint64_t size ) noexcept
	{
	CBASE_PROTECT(
		io->setsize( size );
		return 1;
	)
	}
}
