#include <rsbin/memoryio.hpp>
#include <rsbin/iotask.hpp>
#include <common/ref.hpp>
#include <thread>
#include <stdexcept>
#include <exception>
#include <cstring>

namespace rsbin
{
	enum
	{
		BlockShift = 16,
		BlockSize = 0x10000, // 64K
		BlockOffsetMask = 0xffff,
	};

	MemoryIo::MemoryIo()
		: m_size( 0 )
	{
	}

	MemoryIo::~MemoryIo()
	{
		for( auto& it: m_parts )
		{
			delete[] it;
		}
	}

	IoTask* MemoryIo::startread(
		uint64_t offset, size_t length, void* buffer, bool promote )
	{
		IoTask* task = new IoTask;
		task->m_finished.store( true, std::memory_order_relaxed );
		try
		{
			if( offset > m_size )
			{
				return task;
			}
			if( offset + length > m_size )
			{
				length = m_size - offset;
			}
			while( length != 0 )
			{
				size_t blockindex = offset >> BlockShift;
				ASSERT( blockindex < m_parts.size() );
				size_t boffset = offset & BlockOffsetMask;
				size_t blength = BlockSize - boffset;
				if( blength > length )
				{
					blength = length;
				}
				uint8_t* block = m_parts[ blockindex ];
				if( block )
				{
					memcpy( buffer,
						block + boffset, blength );
				}
				task->m_result += blength;
				offset += blength;
				buffer = ( uint8_t* )buffer + blength;
				length -= blength;
			}
		}
		catch( std::exception const& error )
		{
			task->m_result = 0;
			task->m_error.setchars( error.what() );
			return task;
		}
		catch( ... )
		{
			task->m_result = 0;
			task->m_error.setchars( "unknown error" );
			return task;
		}
		return task;
	}

	IoTask* MemoryIo::startwrite(
		uint64_t offset, size_t length, void const* buffer, bool promote )
	{
		IoTask* task = new IoTask;
		task->m_finished.store( true, std::memory_order_relaxed );
		try
		{
			if( length == 0 )
			{
				return task;
			}
			if( offset + length > m_size )
			{
				m_size = offset + length;
				m_parts.resize(
					( m_size + BlockOffsetMask ) >> BlockShift, 0 );
			}
			while( length != 0 )
			{
				size_t blockindex = offset >> BlockShift;
				ASSERT( blockindex < m_parts.size() );
				size_t boffset = offset & BlockOffsetMask;
				size_t blength = BlockSize - boffset;
				if( blength > length )
				{
					blength = length;
				}
				uint8_t* block = m_parts[ blockindex ];
				if( !block )
				{
					m_parts[ blockindex ] = block = new uint8_t[ BlockSize ];
				}
				memcpy( block + boffset, buffer, blength );
				task->m_result += blength;
				offset += blength;
				buffer = ( uint8_t const* )buffer + blength;
				length -= blength;
			}
		}
		catch( std::exception const& error )
		{
			task->m_result = 0;
			task->m_error.setchars( error.what() );
			return task;
		}
		catch( ... )
		{
			task->m_result = 0;
			task->m_error.setchars( "unknown error" );
			return task;
		}
		return task;
	}

	void MemoryIo::setsize( uint64_t size )
	{
		m_size = size;
		m_parts.resize(
			( size + BlockOffsetMask ) >> BlockShift, 0 );
	}

	uint64_t MemoryIo::getsize()
	{
		return m_size;
	}

	void MemoryIo::getblock(
		uint64_t offset, bool create, uint8_t** pblock, size_t* pblength )
	{
		size_t blockindex = offset >> BlockShift;
		if( blockindex >= m_parts.size() )
		{
			if( create )
			{
				m_parts.resize( blockindex + 1, 0 );
			}
			else
			{
				*pblock = 0;
				return;
			}
		}
		uint8_t* block = m_parts[ blockindex ];
		if( !block )
		{
			if( create )
			{
				m_parts[ blockindex ] = block = new uint8_t[ BlockSize ];
			}
		}
		size_t boffset = offset & BlockOffsetMask;
		*pblock = block + boffset;
		*pblength = BlockSize - boffset;
	}
}
