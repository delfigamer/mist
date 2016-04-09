#pragma once

#include <rsbin/fileio.hpp>
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
	enum class ioaction
	{
		read = 0,
		write = 1,
		truncate = 2,
	};

	R_CLASS( name = fstask )
	class FsTask: public utils::RefObject
	{
	public:
		utils::Ref< FileIo > m_target;
		uint64_t m_offset;
		size_t m_length;
		uint8_t* m_buffer;
		ioaction m_action;
		size_t m_result;
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
		R_METHOD() size_t getresult() NOEXCEPT { return m_result; }
		R_METHOD( stringwrap ) char const* geterror() NOEXCEPT
		{
			return m_error ? m_error.getchars() : 0;
		}
	};

	class FsThreadClass
	{
	private:
		typedef utils::MPSCQueue< utils::Ref< FsTask > > queue_t;
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
		
		void pushhigh( FsTask* task );
		void pushmain( FsTask* task );
	};

	extern utils::Singleton< FsThreadClass > FsThread;
}
