#pragma once

#include <rsbin/io.hpp>
#include <common.hpp>
#include <vector>

namespace rsbin
{
	R_CLASS( name = memoryio )
	class MemoryIo: public Io
	{
	public:
		friend class MemoryReadTask;
		friend class MemoryWriteTask;
		friend class MemoryTruncateTask;
		friend class MemoryGetSizeTask;

	private:
		std::vector< uint8_t* > m_parts;
		uint64_t m_size;

	public:
		MemoryIo();
		virtual ~MemoryIo() override;
		MemoryIo( MemoryIo const& other ) = delete;
		MemoryIo& operator=( MemoryIo const& other ) = delete;

		R_METHOD() static MemoryIo* create()
		{
			return new MemoryIo();
		}
		virtual IoTask* startread(
			uint64_t offset, size_t length, void* buffer,
			bool promote ) override;
		virtual IoTask* startwrite(
			uint64_t offset, size_t length, void const* buffer,
			bool promote ) override;
		virtual void setsize( uint64_t size ) override;
		R_METHOD() uint64_t getsize();
		R_METHOD() void getblock(
			uint64_t offset, bool create, uint8_t** pblock, size_t* pblength );
	};
}
