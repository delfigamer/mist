#include "taskbase.hpp"
#include "cbase.hpp"

namespace utils {
	int const TaskBase::MinPriority;
	int const TaskBase::MaxPriority;
	std::atomic< int > TaskBase::s_order( int( 0 ) );
	
	bool TaskBase::query( int id, void** target ) {
		return queryobject( this, id, target );
	}
	
	TaskBase::TaskBase( int priority ) :
		m_order( s_order.fetch_add( 1, std::memory_order_relaxed ) ) ,
		m_priority( priority ) ,
		m_poschanged( false ) ,
		m_finished( false ) ,
		m_error( std::exception_ptr( nullptr ) ) {
	}
	
	TaskBase::~TaskBase() {
	}
	
	int TaskBase::order() const {
		return m_order.load( std::memory_order_relaxed );
	}
	
	int TaskBase::priority() const {
		return m_priority.load( std::memory_order_relaxed );
	}
	
	void TaskBase::setpriority( int priority ) {
		m_priority.store( priority, std::memory_order_relaxed );
		m_poschanged.store( true, std::memory_order_relaxed );
	}
		
	bool TaskBase::poschanged() const {
		return m_poschanged.load( std::memory_order_relaxed );
	}
	
	void TaskBase::setposchanged( bool poschanged ) {
		m_poschanged.store( poschanged, std::memory_order_relaxed );
	}
	
	bool TaskBase::finished() const {
		return m_finished.load( std::memory_order_relaxed );
	}
	
	void TaskBase::setfinished( bool finished ) {
		m_finished.store( finished, std::memory_order_relaxed );
	}
	
	void TaskBase::error( std::exception_ptr* perror ) {
		*perror = m_error.load( std::memory_order_relaxed );
	}
	
	void TaskBase::seterror( std::exception_ptr const& error ) {
		m_error.store( error, std::memory_order_relaxed );
	}
	
	bool TaskBase::operator<( TaskBase const& other ) const {
		int p1 = priority();
		int p2 = other.priority();
		if( p1 > p2 ) {
			return true;
		} else if( p1 < p2 ) {
			return false;
		} else {
			return order() < other.order();
		}
	}
	
	bool TaskBase::operator>( TaskBase const& other ) const {
		return other < *this;
	}
	
	bool TaskBase::operator<=( TaskBase const& other ) const {
		return !( other < *this );
	}
	
	bool TaskBase::operator>=( TaskBase const& other ) const {
		return !( *this < other );
	}
	
	int utils_taskbase_order( TaskBase* task ) noexcept {
		CBASE_PROTECT(
			return task->order();
		)
	}
	
	int utils_taskbase_priority( TaskBase* task ) noexcept {
		CBASE_PROTECT(
			return task->priority();
		)
	}
	
	bool utils_taskbase_setpriority( TaskBase* task, int priority ) noexcept {
		CBASE_PROTECT(
			task->setpriority( priority );
			return 1;
		)
	}
	
	int utils_taskbase_finished( TaskBase* task ) noexcept {
		CBASE_PROTECT(
			return task->finished() ? 1 : 2;
		)
	}
	
	bool utils_taskbase_flush( TaskBase* task ) noexcept {
		CBASE_PROTECT(
			task->flush();
			return 1;
		)
	}
}