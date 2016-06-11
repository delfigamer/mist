#include <rsbin/fsthread.hpp>
#include <rsbin/fileio.hpp>
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <osapi.hpp>
#include <stdexcept>
#include <exception>
#include <cstring>

namespace rsbin
{
	size_t const BlockSize = 0x1000; // 64K
	size_t const MemBlockSize = 0x1000; // 64K

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
			size_t result = 0;
			if( task->m_action == ioaction::read && task->m_offset < viewsize )
			{
				size_t length = task->m_length;
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
				size_t length = task->m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				DWORD dwresult = 0;
				switch( task->m_action )
				{
				case ioaction::read:
					if( !ReadFile(
						handle, task->m_buffer, DWORD( length ), &dwresult, 0 ) )
					{
						syserror();
					}
					break;

				case ioaction::write:
					if( !WriteFile(
						handle, task->m_buffer, DWORD( length ), &dwresult, 0 ) )
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
				if( task->m_offset + task->m_length > 0x7fffffffULL )
				{
					throw std::runtime_error( "Android does not support files larger than 2GB" );
				}
				int handle = task->m_target->m_handle;
				if( lseek( handle, task->m_offset, SEEK_SET ) == -1 )
				{
					syserror();
				}
				size_t length = task->m_length;
				if( length > BlockSize )
				{
					length = BlockSize;
				}
				switch( task->m_action )
				{
				case ioaction::read:
					result = read( handle, task->m_buffer, length );
					if( result == -1 )
					{
						syserror();
					}
					break;

				case ioaction::write:
					result = write( handle, task->m_buffer, length );
					if( result == -1 )
					{
						syserror();
					}
					break;

				case ioaction::truncate:
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
		queue_t highqueue;
		queue_t mainqueue;
		m_highqueue.store( &highqueue, std::memory_order_release );
		m_mainqueue.store( &mainqueue, std::memory_order_release );
		try
		{
			utils::Ref< FsTask > currenttask;
			while( !m_terminate.load( std::memory_order_relaxed ) )
			{
				if( highqueue.peek( &currenttask ) )
				{
					if( iteratetask( currenttask ) )
					{
						highqueue.pop();
						currenttask->m_finished.store( true,
							std::memory_order_release );
						currenttask = nullptr;
					}
				}
				else if( mainqueue.peek( &currenttask ) )
				{
					if( iteratetask( currenttask ) )
					{
						mainqueue.pop();
						currenttask->m_finished.store( true,
							std::memory_order_release );
						currenttask = nullptr;
					}
				}
				else
				{
					std::this_thread::yield();
				}
			}
		}
		catch( std::exception const& e )
		{
			LOG( "! %s", e.what() );
		}
		catch( ... )
		{
			LOG( "!" );
		}
	}

	FsThreadClass::FsThreadClass()
		: m_terminate( false )
		, m_highqueue( nullptr )
		, m_mainqueue( nullptr )
	{
		m_thread = std::thread( &FsThreadClass::threadfunc, this );
	}

	FsThreadClass::~FsThreadClass()
	{
		finalize();
	}

	void FsThreadClass::finalize()
	{
		if( !m_terminate.load( std::memory_order_relaxed ) )
		{
			m_terminate.store( true, std::memory_order_relaxed );
			m_thread.join();
		}
	}

	void FsThreadClass::pushhigh( FsTask* task )
	{
		queue_t* hq;
		do
		{
			hq = m_highqueue.load( std::memory_order_acquire );
		}
			while( hq == 0 );
		hq->push( task );
	}

	void FsThreadClass::pushmain( FsTask* task )
	{
		queue_t* mq;
		do
		{
			mq = m_mainqueue.load( std::memory_order_acquire );
		}
			while( mq == 0 );
		mq->push( task );
	}

	utils::Singleton< FsThreadClass > FsThread;
}
