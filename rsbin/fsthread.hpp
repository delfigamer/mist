#ifndef RSBIN_FSTHREAD_HPP__
#define RSBIN_FSTHREAD_HPP__ 1

#include <utils/mpscqueue.hpp>
#include <utils/singleton.hpp>
#include <utils/string.hpp>
#include <utils/refobject.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <thread>
#include <atomic>
#include <cstdio>

namespace rsbin
{
	class FileIo;

	enum IoDirection: int
	{
		IoActionRead = 0,
		IoActionWrite = 1,
		IoActionTruncate = 2,
	};

	R_CLASS( name = fstask )
	class FsTask: public utils::RefObject
	{
	public:
		utils::Ref< FileIo > m_target;
		uint64_t m_offset;
		int m_length;
		uint8_t* m_buffer;
		IoDirection m_direction;
		int m_result;
		utils::String m_error;
		std::atomic< bool > m_finished;

		inline FsTask() : m_finished( false ) {}
		virtual ~FsTask() = default;
		FsTask( FsTask const& ) = delete;
		FsTask& operator=( FsTask const& ) = delete;

		R_METHOD() void promote();
		R_METHOD() bool isfinished() NOEXCEPT
		{
			return m_finished.load( std::memory_order_acquire );
		}
		R_METHOD() int getresult() NOEXCEPT { return m_result; }
		R_METHOD( stringwrap ) char const* geterror() NOEXCEPT
		{
			return m_error ? m_error.getchars() : 0;
		}
	};

	class FsThreadClass
	{
	private:
		std::atomic< bool > m_terminate;
		std::thread m_thread;
		utils::MPSCQueue< utils::Ref< FsTask > > m_highqueue;
		utils::MPSCQueue< utils::Ref< FsTask > > m_mainqueue;

		void threadfunc();

	public:
		FsThreadClass();
		~FsThreadClass();
		FsThreadClass( FsThreadClass const& ) = delete;
		FsThreadClass& operator=( FsThreadClass const& ) = delete;

		void pushmain( FsTask* task );
		void pushhigh( FsTask* task );
	};

	extern utils::Singleton< FsThreadClass > FsThread;
}

#endif
