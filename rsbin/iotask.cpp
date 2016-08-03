#include <rsbin/iotask.hpp>
#include <rsbin/fsthread.hpp>

namespace rsbin
{
	IoTask::~IoTask()
	{
	}

	void IoTask::promote()
	{
		FsThread->pushhigh( this );
	}
}

