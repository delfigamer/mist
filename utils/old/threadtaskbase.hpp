#ifndef UTILS_THREADTASKBASE_HPP__
#define UTILS_THREADTASKBASE_HPP__ 1

#include "taskbase.hpp"

namespace utils {
	class ThreadTaskBase: public TaskBase {
	public:
		virtual bool query( int id, void** target ) override;
		ThreadTaskBase() = delete;
		ThreadTaskBase( int priority );
		virtual ~ThreadTaskBase() override;
		ThreadTaskBase( ThreadTaskBase const& other ) = delete;
		ThreadTaskBase( ThreadTaskBase&& other ) = delete;
		ThreadTaskBase& operator=( ThreadTaskBase const& other ) = delete;
		ThreadTaskBase& operator=( ThreadTaskBase&& other ) = delete;
		
		virtual void flush() override;
	};
}

DEFINE_REFID( utils::ThreadTaskBase, utils::TaskBase, 0x487bc5d1 );

#endif
