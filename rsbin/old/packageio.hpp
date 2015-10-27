#ifndef RSBIN_PACKAGEIO_HPP__
#define RSBIN_PACKAGEIO_HPP__ 1

#include "io.hpp"
#include "packageheap.hpp"
#include <utils/ref.hpp>

namespace rsbin {
	class PackageIo: public Io {
	public:
		friend struct PackageIoIoTask;
		
	private:
		utils::Ref< Io > m_base;
		PackageHeap* m_heap;
		PackageHeap::chunk_t const* m_space;
		uint64_t m_maxsize;
		
	public:
		virtual bool query( int id, void** target ) override;
		PackageIo() = delete;
		PackageIo( Io* base, PackageHeap* heap, PackageHeap::chunk_t const* space );
		virtual ~PackageIo() override;
		PackageIo( PackageIo const& other ) = delete;
		PackageIo( PackageIo&& other ) = delete;
		PackageIo& operator=( PackageIo const& other ) = delete;
		PackageIo& operator=( PackageIo&& other ) = delete;
		
		virtual utils::TaskBase* dostartread( uint64_t offset, int length, void* buffer, int* result, int taskpriority ) override;
		virtual utils::TaskBase* dostartwrite( uint64_t offset, int length, void const* buffer, int* result, int taskpriority ) override;
		virtual uint64_t maxsize();
		virtual bool extend( uint64_t minsize );
		virtual PackageHeap::chunk_t const* getspace();
	};
}

DEFINE_REFID( rsbin::PackageIo, rsbin::Io, 0x0b396ef5 )

#endif
