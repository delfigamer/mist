#include <rsbin/formattask.hpp>
#include <stdexcept>

namespace rsbin
{
	void FormatTask::threadfunc()
	{
		try
		{
			process();
		}
		catch( ... )
		{
			m_error = std::current_exception();
		}
		m_finished.store( true, std::memory_order_release );
	}

	void FormatTask::abortpoint()
	{
		if( m_aborted.load( std::memory_order_relaxed ) )
		{
			throw std::runtime_error( "aborted" );
		}
	}

	StorageMap FormatTask::advance( uint32_t length )
	{
		m_maptask = m_stream->startadvance( length );
		if( !m_maptask )
		{
			throw std::runtime_error( "stream failure" );
		}
		while( !m_maptask->poll() )
		{
			abortpoint();
			std::this_thread::yield();
		}
		StorageMap map = m_maptask->getmap();
		if( !map.ptr || map.length == 0 )
		{
			throw std::runtime_error( "stream failure" );
		}
		return map;
	}

	void FormatTask::start()
	{
		m_thread = std::thread( threadfunc, this );
	}

	bool FormatTask::isfinished()
	{
		return m_finished.load( std::memory_order_acquire );
	}

	FormatTask::FormatTask( Stream* stream )
		: m_stream( stream )
		, m_aborted( false )
		, m_finished( false )
	{
		externalassert( stream );
	}

	FormatTask::~FormatTask()
	{
		if( m_thread.joinable() )
		{
			m_aborted.store( true, std::memory_order_relaxed );
			m_thread.join();
		}
	}

	bool FormatTask::poll()
	{
		if( m_finished.load( std::memory_order_acquire ) )
		{
			if( m_error )
			{
				std::rethrow_exception( m_error );
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
}
