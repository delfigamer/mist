#include "fileio.hpp"
#include <utils/strexception.hpp>
#include <utils/ref.hpp>
#include <utils/path.hpp>
#include <utils/console.hpp>
#include <utils/cbase.hpp>
#include <thread>
#include <stdexcept>
#include <exception>
#include <unistd.h>
#include <sys/types.h>
#if defined( _WIN32 ) || defined( _WIN64 )
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <unordered_map>
#endif

// #undef LOG
// #define LOG( ... )

namespace rsbin
{
	int const BlockingReadThreshold = 0x1000; // 4K
#if defined( _WIN32 )
	uint64_t const MaxViewSize = 0x1000000; // 16M
#elif defined( _WIN64 )
	uint64_t const MaxViewSize = 0x10000000000ULL; // 1T
#endif

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

	static void WinError() {
		DWORD LastError = GetLastError();
		if( !LastError ) {
			throw std::runtime_error( "unknown Win32 error" );
		}
		char StrBuffer[ 1024 ];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_MAX_WIDTH_MASK,
			0,
			LastError,
			MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
			StrBuffer,
			sizeof( StrBuffer ) - 1,
			0 );
		throw utils::StrException( "%s", StrBuffer );
	}
#endif
	FileIo::FileIo( char const* path, int mode )
		: m_handle( 0 )
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		if( mode < 0 || mode >= int( sizeof( fp_map ) / sizeof( fp_map[ 0 ] ) ) )
		{
			throw std::runtime_error( "invalid file mode" );
		}
		utils::Ref< utils::Path > upath = utils::Path::create( path );
		FileParameters* fp = &fp_map[ mode ];
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
			WinError();
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
				WinError();
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
#else
		m_handle = fopen( path, mode );
		if( !m_handle )
		{
			perror( path );
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
#else
		fclose( m_handle );
#endif
	}

	FsTask* FileIo::startread( uint64_t offset, int length, void* buffer )
	{
		FsTask* task = new FsTask;
#if defined( _WIN32 ) || defined( _WIN64 )
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
#endif
		task->m_target = this;
		task->m_offset = offset;
		task->m_length = length;
		task->m_buffer = ( uint8_t* )buffer;
		task->m_direction = IoActionRead;
		task->m_result = 0;
		FsThread()->pushmain( task );
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
		FsThread()->pushmain( task );
		return task;
	}

	void FileIo::setsize( uint64_t size )
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		FsTask* task = new FsTask;
		task->m_target = this;
		task->m_offset = size;
		task->m_length = 0;
		task->m_buffer = 0;
		task->m_direction = IoActionTruncate;
		task->m_result = 0;
		FsThread()->pushhigh( task );
		while( !task->m_finished.load( std::memory_order_acquire ) )
		{
			std::this_thread::yield();
		}
		task->release();
#else
#ifdef __ANDROID__
		if( size > 0xffffffffULL )
		{
			throw std::runtime_error( "Android does not support long truncate" );
		}
		if( ftruncate( fileno( m_handle ), size ) != 0 )
		{
#else
		if( ftruncate64( fileno( m_handle ), size ) != 0 )
		{
#endif
			throw std::runtime_error( "ftruncate failed" );
		}
#endif
	}

	extern "C"
	{
		FileIo* rsbin_fileio_new( char const* path, int mode ) noexcept
		{
		CBASE_PROTECT(
			return new FileIo( path, mode );
		)
		}

		FsTask* rsbin_fileio_startread( FileIo* io, uint64_t offset, int length, void* buffer ) noexcept
		{
		CBASE_PROTECT(
			return io->startread( offset, length, buffer );
		)
		}

		FsTask* rsbin_fileio_startwrite( FileIo* io, uint64_t offset, int length, void const* buffer ) noexcept
		{
		CBASE_PROTECT(
			return io->startwrite( offset, length, buffer );
		)
		}

		bool rsbin_fileio_setsize( FileIo* fileio, uint64_t size ) noexcept
		{
		CBASE_PROTECT(
			fileio->setsize( size );
			return 1;
		)
		}
	}
}
