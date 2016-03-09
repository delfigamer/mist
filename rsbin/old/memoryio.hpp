#ifndef RSBIN_MEMORYIO_HPP__
#define RSBIN_MEMORYIO_HPP__ 1

#include "io.hpp"
#include <utils/string.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

namespace rsbin
{
	R_CLASS( name = memoryio )
	class MemoryIo: public Io
	{
	public:
		friend class MemTask;

	private:
		std::vector< uint8_t* > m_parts;
		uint64_t m_size;

	public:
		MemoryIo();
		virtual ~MemoryIo() override;
		MemoryIo( MemoryIo const& other ) = delete;
		MemoryIo& operator=( MemoryIo const& other ) = delete;

		R_METHOD() static MemoryIo* create();
		virtual IoTask* startread(
			uint64_t offset, int length, void* buffer ) override;
		virtual IoTask* startwrite(
			uint64_t offset, int length, void const* buffer ) override;
		virtual void setsize( uint64_t size ) override;
	};
}

#endif
