#ifndef UTILS_THREADTASKQUEUE_HPP__
#define UTILS_THREADTASKQUEUE_HPP__ 1

#include "taskqueuebase.hpp"
#include <thread>
#include <atomic>
#include <exception>

namespace utils {
	class ThreadTaskQueue: public TaskQueueBase {
	private:
		std::thread m_thread;
		std::atomic< bool > m_terminate;
		std::atomic< int > m_sortcounter;
		
		void threadfunc() noexcept;
		
	public:
		ThreadTaskQueue();
		virtual ~ThreadTaskQueue() override;
		ThreadTaskQueue( const ThreadTaskQueue& other ) = delete;
		ThreadTaskQueue( ThreadTaskQueue&& other ) = delete;
		ThreadTaskQueue& operator=( const ThreadTaskQueue& other ) = delete;
		ThreadTaskQueue& operator=( ThreadTaskQueue&& other ) = delete;
		
		virtual void insert( TaskBase* task ) override;
		virtual void iterate() override;
	};
}

#endif
