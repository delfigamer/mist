#include "threadtaskbase.hpp"
#include "console.hpp"
#include <thread>

namespace utils {
	bool ThreadTaskBase::query( int id, void** target ) {
		return queryobject( this, id, target );
	}

	ThreadTaskBase::ThreadTaskBase( int priority ) :
		TaskBase( priority ) {
	}

	ThreadTaskBase::~ThreadTaskBase() {
	}

	void ThreadTaskBase::flush() {
		setpriority( MaxPriority );
		while( !finished() ) {
			std::this_thread::yield();
		}
	}
}
