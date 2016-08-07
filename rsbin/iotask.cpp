#include <rsbin/iotask.hpp>
#include <rsbin/fsthread.hpp>

namespace rsbin
{
	IoTask::~IoTask()
	{
	}

	bool IoTask::iterate()
	{
		return true;
	}
}

