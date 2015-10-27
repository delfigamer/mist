#include "fsthread.hpp"
#include "fileio.hpp"
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <utils/cbase.hpp>
#include <exception>
#include <cstring>
#include <cerrno>
#if defined( _WIN32 ) || defined( _WIN64 )
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

namespace rsbin
{
#if defined( _WIN32 ) || defined( _WIN64 )
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
	int const BlockSize = 0x1000; // 64K
	int const MemBlockSize = 0x1000; // 64K

	// return true if finished
	bool iteratetask( FsTask* task )
	{
		if( task->m_length <= 0 )
		{
			return true;
		}
		try
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			uint64_t viewsize = task->m_target->m_viewsize;
			int result;
			if( task->m_direction == IoActionRead && task->m_offset < viewsize )
			{
				int length = task->m_length;
				if( length > MemBlockSize )
				{
					length = MemBlockSize;
				}
				if( task->m_offset + length > viewsize )
				{
					length = viewsize - task->m_offset;
				}
				memcpy(
					task->m_buffer,
					task->m_target->m_view + task->m_offset,
					length );
				result = length;
			}
			else
			{
				HANDLE handle = ( HANDLE )task->m_target->m_handle;
				LARGE_INTEGER lioffset;
				lioffset.QuadPart = task->m_offset;
				if( !SetFilePointerEx( handle, lioffset, 0, FILE_BEGIN ) )
				{
					WinError();
				}
				int length = task->m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				switch( task->m_direction )
				{
				case IoActionRead:
					if( !ReadFile( handle, task->m_buffer, length, ( LPDWORD )&result, 0 ) )
					{
						WinError();
					}
					break;

				case IoActionWrite:
					if( !WriteFile( handle, task->m_buffer, length, ( LPDWORD )&result, 0 ) )
					{
						WinError();
					}
					break;

				case IoActionTruncate:
					if( !SetEndOfFile( handle ) )
					{
						WinError();
					}
					result = 0;
					break;
				}
			}
			task->m_result += result;
			task->m_offset += result;
			task->m_buffer += result;
			task->m_length -= result;
#else
			FILE* handle = task->m_target->m_handle;
			fsetpos( handle, ( fpos_t* )&task->m_offset );
			int result;
			int length = task->m_length;
			if( length > BlockSize )
			{
				length = BlockSize;
			}
			switch( task->m_direction )
			{
			case IoActionRead:
				result = fread( task->m_buffer, 1, length, handle );
				break;

			case IoActionWrite:
				result = fwrite( task->m_buffer, 1, length, handle );
				break;
			}
			task->m_result += result;
			task->m_offset += result;
			task->m_buffer += result;
			task->m_length -= result;
			if( ferror( handle ) )
			{
				throw utils::StrException( "%s", strerror( errno ) );
				task->m_length = 0;
			}
#endif
			return result == 0 || task->m_length == 0;
		}
		catch( std::exception const& error )
		{
			task->m_result = 0;
			task->m_error.setchars( error.what() );
			return true;
		}
	}

	void FsThreadClass::threadfunc()
	{
		utils::Ref< FsTask > currenttask;
		while( !m_terminate.load( std::memory_order_relaxed ) )
		{
			if( m_highqueue.peek( &currenttask ) )
			{
				if( iteratetask( currenttask ) )
				{
					m_highqueue.pop();
					currenttask->m_finished.store( true, std::memory_order_release );
					currenttask = nullptr;
				}
			}
			else if( m_mainqueue.peek( &currenttask ) )
			{
				if( iteratetask( currenttask ) )
				{
					m_mainqueue.pop();
					currenttask->m_finished.store( true, std::memory_order_release );
					currenttask = nullptr;
				}
			}
		}
	}

	FsThreadClass::FsThreadClass()
		: m_terminate( false )
		, m_thread( &FsThreadClass::threadfunc, this )
	{
	}

	FsThreadClass::~FsThreadClass()
	{
		m_terminate.store( true, std::memory_order_relaxed );
		m_thread.join();
	}

	void FsThreadClass::pushhigh( FsTask* task )
	{
		m_highqueue.push( task );
	}

	void FsThreadClass::pushmain( FsTask* task )
	{
		m_mainqueue.push( task );
	}

	utils::Singleton< FsThreadClass > FsThread;

	extern "C"
	{
		bool rsbin_fstask_promote( FsTask* task ) noexcept
		{
		CBASE_PROTECT(
			FsThread()->pushhigh( task );
			return 1;
		)
		}

		int rsbin_fstask_isfinished( FsTask* task ) noexcept
		{
		CBASE_PROTECT(
			return task->m_finished.load( std::memory_order_acquire ) ? 1 : 2;
		)
		}

		int rsbin_fstask_getresult( FsTask* task ) noexcept
		{
		CBASE_PROTECT(
			return task->m_result;
		)
		}

		char const* rsbin_fstask_geterror( FsTask* task ) noexcept
		{
		CBASE_PROTECT(
			return task->m_error.getchars();
		)
		}
	}
}
