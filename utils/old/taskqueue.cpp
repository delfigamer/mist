#include "taskqueue.hpp"
#include "cbase.hpp"

namespace utils {
	TaskQueue::TaskQueue() {
	}
	
	TaskQueue::~TaskQueue() {
	}
	
	void TaskQueue::iterate() {
		lock_t _mutexlock( m_mutex );
		taskset_t::iterator it = m_taskset.begin();
		taskset_t::iterator eit = m_taskset.end();
		while( it != eit ) {
			TaskBase* task = *it;
			if( !task->finished() ) {
				task->action();
			}
			if( task->finished() ) {
				it = m_taskset.erase( it );
			} else {
				if( task->poschanged() ) {
					m_taskstack.push( task );
					it = m_taskset.erase( it );
				} else {
					++it;
				}
			}
		}
		while( !m_taskstack.empty() ) {
			m_taskset.insert( m_taskstack.top() );
			m_taskstack.pop();
		}
	}
	
	TaskQueue* utils_taskqueue_new() noexcept {
		CBASE_PROTECT(
			return new TaskQueue;
		)
	}
	
	TaskQueue* utils_taskqueue_default() noexcept {
		CBASE_PROTECT(
			return DefaultQueue;
		)
	}
	
	Ref< TaskQueue > DefaultQueue = Ref< TaskQueue >::create();
}
