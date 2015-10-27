#include "threadtaskqueue.hpp"
#include "threadtaskbase.hpp"
#include "console.hpp"
#include <stdexcept>
#include <utility>
#include <cinttypes>

namespace utils {
	void ThreadTaskQueue::threadfunc() noexcept {
		LOG( "~ Task thread 0x%" PRIxPTR " started", uintptr_t( this ) );
		TaskBase* taskref = 0;
		while( !m_terminate.load( std::memory_order_relaxed ) ) {
			if( m_sortcounter.fetch_sub( 100, std::memory_order_relaxed ) < 0 ) {
				iterate();
			}
			{
				lock_t _mutexlock( m_mutex );
				taskset_t::iterator it = m_taskset.begin();
				taskset_t::iterator eit = m_taskset.end();
				if( it != eit ) {
					( *it )->action();
					if( ( *it )->finished() ) {
						taskref = *it;
						taskref->addref();
						m_taskset.erase( it );
					}
				}
			}
			if( taskref ) {
				taskref->release();
				taskref = 0;
			}
		}
		LOG( "~ Task thread 0x%" PRIxPTR " finished", uintptr_t( this ) );
	}

	ThreadTaskQueue::ThreadTaskQueue() :
		m_terminate( false ) ,
		m_sortcounter( 0 ) {
		m_thread = std::thread( &ThreadTaskQueue::threadfunc, this );
	}

	ThreadTaskQueue::~ThreadTaskQueue() {
		m_terminate.store( true, std::memory_order_relaxed );
		LOG( "~ Waiting for a task thread 0x%" PRIxPTR "", uintptr_t( this ) );
		m_thread.join();
		LOG( "~ Task thread 0x%" PRIxPTR " joined", uintptr_t( this ) );
	}

	void ThreadTaskQueue::insert( TaskBase* task ) {
		Ref< ThreadTaskBase > tref = task;
		TaskQueueBase::insert( task );
	}

	void ThreadTaskQueue::iterate() {
		lock_t _mutexlock( m_mutex );
		int sortcounter = m_sortcounter.load( std::memory_order_relaxed );
		taskset_t::iterator it = m_taskset.begin();
		taskset_t::iterator eit = m_taskset.end();
		while( it != eit ) {
			sortcounter += 10;
			if( ( *it )->poschanged() ) {
				m_taskstack.push( std::move( *it ) );
				it = m_taskset.erase( it );
			} else {
				++it;
			}
		}
		while( !m_taskstack.empty() ) {
			sortcounter -= 4;
			m_taskset.insert( std::move( m_taskstack.top() ) );
			m_taskstack.pop();
		}
		if( sortcounter < 0 ) {
			sortcounter = 0;
		}
		m_sortcounter.store( sortcounter, std::memory_order_relaxed );
	}
}
