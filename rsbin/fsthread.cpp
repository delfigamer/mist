#include <rsbin/fsthread.hpp>
#include <common/strexception.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <exception>

namespace rsbin
{
	void FsThreadClass::threadfunc()
	{
		queue_t highqueue;
		queue_t mainqueue;
		m_highqueue.store( &highqueue, std::memory_order_release );
		m_mainqueue.store( &mainqueue, std::memory_order_release );
		try
		{
			Ref< IoTask > currenttask;
			while( !m_terminate.load( std::memory_order_relaxed ) )
			{
				if( highqueue.peek( &currenttask ) )
				{
					if( currenttask->iterate() )
					{
						highqueue.pop();
						currenttask->m_finished.store( true,
							std::memory_order_release );
						currenttask = nullptr;
					}
				}
				else if( mainqueue.peek( &currenttask ) )
				{
					if( currenttask->iterate() )
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
		m_terminate.store( true, std::memory_order_relaxed );
		m_thread.join();
	}

	void FsThreadClass::pushhigh( IoTask* task )
	{
		queue_t* hq;
		do
		{
			hq = m_highqueue.load( std::memory_order_acquire );
		}
			while( hq == 0 );
		hq->push( task );
	}

	void FsThreadClass::pushmain( IoTask* task )
	{
		queue_t* mq;
		do
		{
			mq = m_mainqueue.load( std::memory_order_acquire );
		}
			while( mq == 0 );
		mq->push( task );
	}

	FsThreadClass* FsThread;
}
