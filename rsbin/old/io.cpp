#include "io.hpp"
#include <utils/taskqueue.hpp>
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <thread>
#include <exception>

namespace rsbin {
	bool Io::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	Io::Io() {
	}

	Io::~Io() {
	}

	utils::Ref< utils::TaskBase > Io::startread( uint64_t offset, int length, void* buffer, int* result, int taskpriority ) {
		return utils::Ref< utils::TaskBase >( dostartread( offset, length, buffer, result, taskpriority ), 0 );
	}

	utils::Ref< utils::TaskBase > Io::startwrite( uint64_t offset, int length, void const* buffer, int* result, int taskpriority ) {
		return utils::Ref< utils::TaskBase >( dostartwrite( offset, length, buffer, result, taskpriority ), 0 );
	}

	int Io::syncread( uint64_t offset, int length, void* buffer ) {
		int result;
		utils::Ref< utils::TaskBase > task = startread( offset, length, buffer, &result, utils::TaskBase::MaxPriority );
		task->flush();
		task->release();
		return result;
	}

	int Io::syncwrite( uint64_t offset, int length, void const* buffer ) {
		int result;
		utils::Ref< utils::TaskBase > task = startwrite( offset, length, buffer, &result, utils::TaskBase::MaxPriority );
		task->flush();
		task->release();
		return result;
	}

	extern "C" {
		utils::TaskBase* rsbin_io_startread( Io* io, uint64_t offset, int length, void* buffer, int* result, int taskpriority ) noexcept {
		CBASE_PROTECT(
			return io->dostartread( offset, length, buffer, result, taskpriority );
		)
		}

		utils::TaskBase* rsbin_io_startwrite( Io* io, uint64_t offset, int length, void const* buffer, int* result, int taskpriority ) noexcept {
		CBASE_PROTECT(
			return io->dostartwrite( offset, length, buffer, result, taskpriority );
		)
		}
	}
}
