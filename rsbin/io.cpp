#include <rsbin/io.hpp>
#include <rsbin/iotask.hpp>
#include <common/strexception.hpp>

namespace rsbin
{
	Io::Io()
	{
	}

	Io::~Io()
	{
	}

	uint64_t Io::blockread(
		uint64_t offset, size_t length, void* buffer, bool promote )
	{
		Ref< IoTask > task( startread( offset, length, buffer, promote ), 0 );
		while( !task->m_finished.load( std::memory_order_acquire ) )
		{
		}
		if( task->m_error )
		{
			StrException error( task->m_error );
			throw error;
		}
		else
		{
			uint64_t result = task->m_result;
			return result;
		}
	}

	uint64_t Io::blockwrite(
		uint64_t offset, size_t length, void const* buffer, bool promote )
	{
		Ref< IoTask > task( startwrite( offset, length, buffer, promote ), 0 );
		while( !task->m_finished.load( std::memory_order_acquire ) )
		{
		}
		if( task->m_error )
		{
			StrException error( task->m_error );
			throw error;
		}
		else
		{
			uint64_t result = task->m_result;
			return result;
		}
	}
}
