#include "iotask.hpp"
#include "fsthread.hpp"

namespace rsbin
{
	void IoTask::promote()
	{
		FsThread->pushhigh( this );
	}
}

