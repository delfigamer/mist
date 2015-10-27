#include "taskqueuebase.hpp"
#include "cbase.hpp"

namespace utils {
	bool TaskQueueBase::query( int id, void** target ) {
		return queryobject( this, id, target );
	}
	
	TaskQueueBase::TaskQueueBase() {
	}
	
	TaskQueueBase::~TaskQueueBase() {
	}
	
	void TaskQueueBase::insert( TaskBase* task ) {
		lock_t _mutexlock( m_mutex );
		m_taskset.emplace( task );
	}
		
	bool TaskQueueBase::empty() const {
		lock_t _mutexlock( m_mutex );
		return m_taskset.empty();
	}
	
	bool utils_taskqueuebase_insert( TaskQueueBase* queue, TaskBase* task ) noexcept {
		CBASE_PROTECT(
			queue->insert( task );
			return 1;
		)
	}
	
	bool utils_taskqueuebase_iterate( TaskQueueBase* queue ) noexcept {
		CBASE_PROTECT(
			queue->iterate();
			return 1;
		)
	}
	
	int utils_taskqueuebase_empty( TaskQueueBase* queue ) noexcept {
		CBASE_PROTECT(
			return queue->empty() ? 1 : 2;
		)
	}
}
