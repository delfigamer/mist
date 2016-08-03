#include <rsbin/memoryio.hpp>
#include <rsbin/fsthread.hpp>
#include <utils/ref.hpp>
#include <thread>
#include <stdexcept>
#include <exception>
#include <cstring>

namespace rsbin
{
	enum
	{
		BlockingReadThreshold = 0x1000, // 4K
		BlockShift = 4,
		BlockSize = 0x10, // 64K
		BlockOffsetMask = 0xf,
	};

	class MemoryReadTask: public IoTask
	{
	public:
		utils::Ref< MemoryIo > m_target;
		uint64_t m_offset;
		size_t m_length;
		uint8_t* m_buffer;

		virtual bool iterate() override
		{
			try
			{
				if( m_offset + m_length > m_target->m_size )
				{
					m_length = m_target->m_size - m_offset;
				}
				if( m_length <= 0 )
				{
					return true;
				}
				size_t blockindex = m_offset >> BlockShift;
				if( blockindex >= m_target->m_parts.size() )
				{
					return true;
				}
				size_t boffset = m_offset & BlockOffsetMask;
				size_t length = BlockSize - boffset;
				if( length > m_length )
				{
					length = m_length;
				}
				uint8_t* block = m_target->m_parts[ blockindex ];
				if( block )
				{
					memcpy( m_buffer,
						block + boffset, length );
				}
				m_result += length;
				m_offset += length;
				m_buffer += length;
				m_length -= length;
				return m_length == 0;
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
	};

	struct MemoryWriteTask: public IoTask
	{
		utils::Ref< MemoryIo > m_target;
		uint64_t m_offset;
		size_t m_length;
		uint8_t* m_buffer;

		virtual bool iterate() override
		{
			try
			{
				if( m_length <= 0 )
				{
					return true;
				}
				if( m_offset + m_length > m_target->m_size )
				{
					m_target->m_size = m_offset + m_length;
					m_target->m_parts.resize(
						( m_target->m_size + BlockOffsetMask ) >> BlockShift, 0 );
				}
				size_t blockindex = m_offset >> BlockShift;
				if( blockindex >= m_target->m_parts.size() )
				{
					return true;
				}
				size_t boffset = m_offset & BlockOffsetMask;
				size_t length = BlockSize - boffset;
				if( length > m_length )
				{
					length = m_length;
				}
				uint8_t* block = m_target->m_parts[ blockindex ];
				if( !block )
				{
					m_target->m_parts[ blockindex ] = block =
						new uint8_t[ BlockSize ];
				}
				memcpy( m_target->m_parts[ blockindex ] + boffset,
					m_buffer, length );
				m_result += length;
				m_offset += length;
				m_buffer += length;
				m_length -= length;
				return m_length == 0;
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
	};

	struct MemoryTruncateTask: public IoTask
	{
		utils::Ref< MemoryIo > m_target;
		uint64_t m_newsize;

		virtual bool iterate() override
		{
			try
			{
				m_target->m_size = m_newsize;
				m_target->m_parts.resize(
					( m_newsize + BlockOffsetMask ) >> BlockShift, 0 );
				return true;
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
	};

	MemoryIo::MemoryIo()
		: m_size( 0 )
	{
	}

	MemoryIo::~MemoryIo()
	{
		for( auto& it: m_parts )
		{
			delete it;
		}
	}

	IoTask* MemoryIo::startread(
		uint64_t offset, size_t length, void* buffer )
	{
		MemoryReadTask* task = new MemoryReadTask;
		task->m_target = this;
		task->m_offset = offset;
		task->m_length = length;
		task->m_buffer = ( uint8_t* )buffer;
		if( length < BlockingReadThreshold )
		{
			FsThread->pushhigh( task );
			while( !task->m_finished.load( std::memory_order_acquire ) )
			{
				// std::this_thread::yield();
			}
		}
		else
		{
			FsThread->pushmain( task );
		}
		return task;
	}

	IoTask* MemoryIo::startwrite(
		uint64_t offset, size_t length, void const* buffer )
	{
		MemoryWriteTask* task = new MemoryWriteTask;
		task->m_target = this;
		task->m_offset = offset;
		task->m_length = length;
		task->m_buffer = ( uint8_t* )buffer;
		FsThread->pushmain( task );
		return task;
	}

	void MemoryIo::setsize( uint64_t size )
	{
		MemoryTruncateTask* task = new MemoryTruncateTask;
		task->m_target = this;
		task->m_newsize = size;
		FsThread->pushhigh( task );
		while( !task->m_finished.load( std::memory_order_acquire ) )
		{
			// std::this_thread::yield();
		}
		task->release();
	}
}
