#pragma once

#include <rsbin/iotask.hpp>
#include <utils/mpscqueue.hpp>
#include <utils/singleton.hpp>
#include <utils/string.hpp>
#include <utils/refobject.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <thread>
#include <atomic>

namespace rsbin
{
	class FsThreadClass
	{
	private:
		typedef utils::MPSCQueue< utils::Ref< IoTask > > queue_t;

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
		void finalize();

		void pushhigh( IoTask* task );
		void pushmain( IoTask* task );
	};

	extern utils::Singleton< FsThreadClass > FsThread;
}
