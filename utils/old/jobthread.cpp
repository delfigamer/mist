#include "jobthread.hpp"

#define synchronize() std::lock_guard< std::mutex > _lock_guard( m_mutex )

namespace utils {
	Job::Job(
		void* _data,
		void( *_action )( void* ),
		void( *_destroy )( void* ),
		void( *_error )( void*, std::exception_ptr&& eptr ) ) :
		data( _data ) ,
		action( _action ) ,
		destroy( _destroy ) ,
		error( _error ) {
	}
	
	void JobThread::threadfunc() noexcept {
		Job job;
		while( !m_terminate ) {
			next( &job );
			invoke( job );
		}
		while( pop( &job ) ) {
			finalize( job );
		}
	}
	
	bool JobThread::pop( Job* job ) noexcept {
		synchronize();
		if( m_queue.empty() ) {
			return false;
		} else {
			*job = m_queue.front();
			m_queue.pop();
			return true;
		}
	}
	
	void JobThread::next( Job* job ) noexcept {
		synchronize();
		if( m_queue.empty() ) {
			*job = m_idle;
		} else {
			*job = m_queue.front();
			m_queue.pop();
		}
	}
	
	void JobThread::invoke( Job const& job ) noexcept {
		try {
			if( job.action ) {
				job.action( job.data );
			}
		} catch( ... ) {
			notify( job, std::current_exception() );
		}
		finalize( job );
	}
	
	void JobThread::notify( Job const& job, std::exception_ptr&& eptr ) noexcept {
		if( job.error ) {
			try {
				job.error( job.data, std::move( eptr ) );
			} catch( ... ) {
			}
		}
	}
	
	void JobThread::finalize( Job const& job ) noexcept {
		if( job.destroy ) {
			try {
				job.destroy( job.data );
			} catch( ... ) {
			}
		}
	}
	
	void JobThread::yield( void* ) {
		std::this_thread::yield();
	}
	
	JobThread::JobThread() :
		m_idle( this, &JobThread::yield ) ,
		m_terminate( false ) {
		m_thread = std::thread( &JobThread::threadfunc, this );
	}
	
	JobThread::~JobThread() {
		m_terminate = true;
		m_thread.join();
	}
	
	void JobThread::push(
		void* data,
		void( *action )( void* ),
		void( *destroy )( void* ),
		void( *error )( void*, std::exception_ptr&& eptr ) ) {
		synchronize();
		m_queue.push( Job( data, action, destroy, error ) );
	}
	
	void JobThread::push( Job const& job ) {
		synchronize();
		m_queue.push( job );
	}
	
	void JobThread::setidle( Job const& job ) {
		synchronize();
		m_idle = job;
	}
	
	void JobThread::setidle() {
		synchronize();
		m_idle = Job( this, &JobThread::yield );
	}
	
	bool JobThread::empty() {
		synchronize();
		return m_queue.empty();
	}
}
