#include "fsthread.hpp"
#include "fileio.hpp"
#include <utils/strexception.hpp>
#include <utils/cbase.hpp>
#include <osapi.hpp>
#include <stdexcept>
#include <exception>
#include <cstring>

namespace rsbin
{
	int const BlockSize = 0x1000; // 64K
	int const MemBlockSize = 0x1000; // 64K

	void FsTask::promote()
	{
		FsThread->pushhigh( this );
	}

	// return true if finished
	bool iteratetask( FsTask* task )
	{
		if( task->m_length <= 0 )
		{
			return true;
		}
		try
		{
			uint64_t viewsize = task->m_target->m_viewsize;
			int result = 0;
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
#if defined( _WIN32 ) || defined( _WIN64 )
				HANDLE handle = ( HANDLE )task->m_target->m_handle;
				LARGE_INTEGER lioffset;
				lioffset.QuadPart = task->m_offset;
				if( !SetFilePointerEx( handle, lioffset, 0, FILE_BEGIN ) )
				{
					syserror();
				}
				int length = task->m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				switch( task->m_direction )
				{
				case IoActionRead:
					if( !ReadFile(
						handle, task->m_buffer, length, ( LPDWORD )&result, 0 ) )
					{
						syserror();
					}
					break;

				case IoActionWrite:
					if( !WriteFile(
						handle, task->m_buffer, length, ( LPDWORD )&result, 0 ) )
					{
						syserror();
					}
					break;

				case IoActionTruncate:
					if( !SetEndOfFile( handle ) )
					{
						syserror();
					}
					break;
				}
#elif defined( __ANDROID__ )
				if( task->m_offset + task->m_length > 0x7fffffffULL )
				{
					throw std::runtime_error( "Android does not support files larger than 2GB" );
				}
				int handle = task->m_target->m_handle;
				if( lseek( handle, task->m_offset, SEEK_SET ) == -1 )
				{
					syserror();
				}
				int length = task->m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				switch( task->m_direction )
				{
				case IoActionRead:
					result = read( handle, task->m_buffer, length );
					if( result == -1 )
					{
						syserror();
					}
					break;

				case IoActionWrite:
					result = write( handle, task->m_buffer, length );
					if( result == -1 )
					{
						syserror();
					}
					break;

				case IoActionTruncate:
					if( ftruncate( handle, task->m_offset ) == -1 )
					{
						syserror();
					}
					break;
				}
#endif
			}
			task->m_result += result;
			task->m_offset += result;
			task->m_buffer += result;
			task->m_length -= result;
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
			else
			{
				std::this_thread::yield();
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
}
