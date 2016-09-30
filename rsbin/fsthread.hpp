#pragma once

#include <rsbin/iotask.hpp>
#include <common/mpscqueue.hpp>
#include <common/string.hpp>
#include <common/refobject.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <thread>
#include <atomic>

namespace rsbin
{
	class FsThreadClass
	{
	private:
		typedef MPSCQueue< Ref< IoTask > > queue_t;

	private:
		std::atomic< bool > m_terminate;
		std::thread m_thread;
		std::atomic< queue_t* > m_highqueue;
		std::atomic< queue_t* > m_mainqueue;

		void threadfunc();

	public:
		FsThreadClass();
		~FsThreadClass();
		FsThreadClass( FsThreadClass const& ) = delete;
		FsThreadClass& operator=( FsThreadClass const& ) = delete;

		void pushhigh( IoTask* task );
		void pushmain( IoTask* task );
	};

	extern FsThreadClass* FsThread;
}
