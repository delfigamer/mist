#ifndef UTILS_TASKSEQUENCE_HPP__
#define UTILS_TASKSEQUENCE_HPP__ 1

#include "taskbase.hpp"
#include "taskqueue.hpp"

namespace utils {
	template< typename First, typename ...Others >
	struct TypeTuple {
		typedef First type;
		typedef TypeTuple< Others... > next;
		enum {
			length = next::length + 1
		};
	};
	
	template< typename First >
	struct TypeTuple< First > {
		typedef First type;
		typedef void next;
		enum {
			length = 1
		};
	};
	
	template< typename TT, int index >
	struct GetType {
		typedef typename GetType< typename TT::next, index - 1 >::type type;
	};
	
	template< typename TT >
	struct GetType< TT, 0 > {
		typedef typename TT::type type;
	};
	
	template< typename ...Ts >
	class TaskSequence: public TaskBase {
	private:
		typedef TypeTuple< Ts... > tasks;
		
	private:
		Ref< TaskBase > m_subtask;
		int m_index;
		
	public:
		TaskSequence() = delete;
		TaskSequence( int order, int priority );
		virtual ~TaskSequence() override;
		TaskSequence( TaskSequence const& other ) = delete;
		TaskSequence( TaskSequence&& other ) = delete;
		TaskSequence& operator=( TaskSequence const& other ) = delete;
		TaskSequence& operator=( TaskSequence&& other ) = delete;
		
		virtual void setpriority( int priority ) override;
		virtual void action() override;
	};
	
	template< typename ...Ts >
	TaskSequence< Ts... >::TaskSequence( int order, int priority ) :
		TaskBase( order, priority ) ,
		m_index( 0 ) {
	}
	
	template< typename ...Ts >
	TaskSequence< Ts... >::~TaskSequence() {
	}
	
	template< typename ...Ts >
	void TaskSequence< Ts... >::setpriority( int priority ) {
		TaskBase::setpriority( priority );
		if( m_subtask ) {
			m_subtask->setpriority( priority );
		}
	}
	
	template< typename ...Ts >
	void TaskSequence< Ts... >::action() {
		if( m_subtask && !m_subtask->finished() ) {
			return;
		}
		if( m_index == tasks::length ) {
			setfinished( true );
			return;
		}
		m_subtask = 
	}
}

#endif
