#ifndef UTILS_TASKQUEUEBASE_HPP__
#define UTILS_TASKQUEUEBASE_HPP__ 1

#include "taskbase.hpp"
#include "refobject.hpp"
#include "ref.hpp"
#include "monitor.hpp"
#include <set>
#include <stack>
#include <vector>

namespace utils {
	class TaskQueueBase: public Monitor {
	protected:
		typedef std::multiset< Ref< TaskBase >, TaskComparer > taskset_t;
		typedef std::stack< Ref< TaskBase >, std::vector< Ref< TaskBase > > > taskstack_t;
		
	protected:
		taskset_t m_taskset;
		taskstack_t m_taskstack;
		
	public:
		virtual bool query( int id, void** target ) override;
		TaskQueueBase();
		virtual ~TaskQueueBase() override;
		TaskQueueBase( TaskQueueBase const& other ) = delete;
		TaskQueueBase( TaskQueueBase&& other ) = delete;
		TaskQueueBase& operator=( TaskQueueBase const& other ) = delete;
		TaskQueueBase& operator=( TaskQueueBase&& other ) = delete;
		
		virtual void insert( TaskBase* task );
		virtual void iterate() = 0;
		bool empty() const;
	};
}

DEFINE_REFID( utils::TaskQueueBase, utils::RefObject, 0x8da7cba4 )

namespace utils {
	extern "C" {
		bool utils_taskqueuebase_insert( TaskQueueBase* queue, TaskBase* task ) noexcept;
		bool utils_taskqueuebase_iterate( TaskQueueBase* queue ) noexcept;
		int utils_taskqueuebase_empty( TaskQueueBase* queue ) noexcept;
	}
}

#endif
