#ifndef UTILS_POOLALLOCATOR_HPP__
#define UTILS_POOLALLOCATOR_HPP__ 1

#include "counterlock.hpp"
#include "flaglock.hpp"
#include <mutex>
#include <vector>

namespace utils {
	class PoolAllocator {
	public:
		static const size_t DefaultPageSize = 0x400;
		static const size_t MinPageSize = 0x100;
		static const size_t MaxPageSize = 0x10000;
		static const size_t PageSizeMask = ~0xff;

	private:
		typedef CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		typedef std::vector< uint8_t* > pages_t;

	private:
		mutex_t m_mutex;
		size_t m_itemsize;
		size_t m_itemcount;
		pages_t m_pages;
		uint8_t* m_top;

		static uint8_t* getlink( uint8_t const* ptr );
		static void setlink( uint8_t* ptr, uint8_t const* link );
		static uint8_t* newpage( uint8_t* nextptr, size_t itemsize, size_t itemcount );

	public:
		PoolAllocator() = delete;
		PoolAllocator( size_t itemsize );
		PoolAllocator( size_t itemsize, size_t itemcount );
		PoolAllocator( PoolAllocator const& ) = delete;
		PoolAllocator( PoolAllocator&& ) = delete;
		~PoolAllocator();

		void* alloc();
		void free( void* ptr );
	};
}

#endif
