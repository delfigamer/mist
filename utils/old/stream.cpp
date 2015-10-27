#include "stream.hpp"
#include <utils/taskqueue.hpp>
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <exception>

namespace utils {
	bool Stream::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	Stream::Stream() {
	}

	Stream::~Stream() {
	}

	Ref< utils::TaskBase > Stream::starttransfer( int length, void* buffer, int* result, int taskpriority ) {
		return Ref< TaskBase >( dostarttransfer( length, buffer, result, taskpriority ), 0 );
	}

	int Stream::synctransfer( int length, void* buffer ) {
		int result;
		Ref< TaskBase > task = starttransfer( length, buffer, &result, TaskBase::MaxPriority );
		task->flush();
		task = nullptr;
		return result;
	}

	extern "C" {
		TaskBase* utils_stream_starttransfer( Stream* stream, int length, void* buffer, int* result, int taskpriority ) noexcept {
		CBASE_PROTECT(
			return stream->dostarttransfer( length, buffer, result, taskpriority );
		)
		}
	}
}
