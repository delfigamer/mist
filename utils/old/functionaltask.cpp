#include "functionaltask.hpp"

namespace utils {
	bool FunctionalTask::query( int id, void** target ) {
		return queryobject( this, id, target );
	}
	
	FunctionalTask::FunctionalTask( int priority ) :
		TaskBase( priority ) {
	}
	
	FunctionalTask::~FunctionalTask() {
	}
	
	void FunctionalTask::setpriority( int priority ) {
		TaskBase::setpriority( priority );
		if( m_subtask ) {
			m_subtask->setpriority( priority );
		}
	}
	
	void FunctionalTask::flush() {
		while( !finished() ) {
			if( m_subtask ) {
				m_subtask->flush();
				std::exception_ptr err;
				m_subtask->error( &err );
				if( err ) {
					seterror( err );
					setfinished( true );
					std::rethrow_exception( err );
				}
			}
			try {
				doaction();
			} catch( ... ) {
				seterror( std::current_exception() );
				setfinished( true );
				throw;
			}
		}
	}
	
	void FunctionalTask::action() {
		if( m_subtask ) {
			if( !m_subtask->finished() ) {
				return;
			}
			std::exception_ptr err;
			m_subtask->error( &err );
			if( err ) {
				seterror( err );
				setfinished( true );
				return;
			}
		}
		try {
			doaction();
		} catch( ... ) {
			seterror( std::current_exception() );
			setfinished( true );
		}
	}
}
