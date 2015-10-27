#ifndef UTILS_TASKQUEUE_HPP__
#define UTILS_TASKQUEUE_HPP__ 1

#include "taskqueuebase.hpp"

namespace utils {
	class TaskQueue: public TaskQueueBase {
	public:
		TaskQueue();
		virtual ~TaskQueue() override;
		TaskQueue( TaskQueue const& other ) = delete;
		TaskQueue( TaskQueue&& other ) = delete;
		TaskQueue& operator=( TaskQueue const& other ) = delete;
		TaskQueue& operator=( TaskQueue&& other ) = delete;
		
		virtual void iterate() override;
	};
	
	extern "C" {
		TaskQueue* utils_taskqueue_new() noexcept;
		TaskQueue* utils_taskqueue_default() noexcept;
	}
	
	extern Ref< TaskQueue > DefaultQueue;
}

#endif
