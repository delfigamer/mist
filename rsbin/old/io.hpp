#ifndef RSBIN_IO_HPP__
#define RSBIN_IO_HPP__ 1

#include <utils/taskbase.hpp>
#include <utils/refobject.hpp>
#include <cstdio>
#include <cinttypes>

namespace rsbin {
	class Io: public utils::RefObject {
	public:
		virtual bool query( int id, void** target ) override;
		Io();
		virtual ~Io() override;
		Io( Io const& other ) = delete;
		Io( Io&& other ) = delete;
		Io& operator=( Io const& other ) = delete;
		Io& operator=( Io&& other ) = delete;
		
		virtual utils::TaskBase* dostartread( uint64_t offset, int length, void* buffer, int* result, int taskpriority ) = 0;
		virtual utils::TaskBase* dostartwrite( uint64_t offset, int length, void const* buffer, int* result, int taskpriority ) = 0;
		utils::Ref< utils::TaskBase > startread( uint64_t offset, int length, void* buffer, int* result, int taskpriority );
		utils::Ref< utils::TaskBase > startwrite( uint64_t offset, int length, void const* buffer, int* result, int taskpriority );
		int syncread( uint64_t offset, int length, void* buffer );
		int syncwrite( uint64_t offset, int length, void const* buffer );
	};
}

DEFINE_REFID( rsbin::Io, utils::RefObject, 0x658374e5 )

namespace rsbin {
	extern "C" {
		utils::TaskBase* rsbin_io_startread( Io* io, uint64_t offset, int length, void* buffer, int* result, int taskpriority ) noexcept;
		utils::TaskBase* rsbin_io_startwrite( Io* io, uint64_t offset, int length, void const* buffer, int* result, int taskpriority ) noexcept;
	}
}

#endif
