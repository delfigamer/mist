#ifndef UTILS_FUNCTIONALTASK_HPP__
#define UTILS_FUNCTIONALTASK_HPP__ 1

#include "taskbase.hpp"

namespace utils {
	class FunctionalTask: public TaskBase {
	protected:
		Ref< TaskBase > m_subtask;
		
	public:
		virtual bool query( int id, void** target ) override;
		FunctionalTask() = delete;
		FunctionalTask( int priority );
		virtual ~FunctionalTask() override;
		FunctionalTask( FunctionalTask const& other ) = delete;
		FunctionalTask( FunctionalTask&& other ) = delete;
		FunctionalTask& operator=( FunctionalTask const& other ) = delete;
		FunctionalTask& operator=( FunctionalTask&& other ) = delete;
		
		virtual void setpriority( int priority ) override;
		virtual void flush() override;
		virtual void action() override;
		virtual void doaction() = 0;
	};
}

DEFINE_REFID( utils::FunctionalTask, utils::TaskBase, 0x68530d13 );

#endif
