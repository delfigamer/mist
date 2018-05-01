#pragma once

#include <rsbin/stream.hpp>
#include <common/ref.hpp>
#include <common/task.hpp>
#include <common.hpp>
#include <exception>
#include <atomic>
#include <thread>

namespace rsbin
{
	class [[ r::class, r::name( "formattask" ) ]] FormatTask: public Task
	{
	private:
		std::thread m_thread;
		Ref< Stream > m_stream;
		Ref< MapTask > m_maptask;
		std::atomic< bool > m_aborted;
		std::atomic< bool > m_finished;

	protected:
		std::exception_ptr m_error;

	private:
		void threadfunc();

	protected:
		void abortpoint();
		StorageMap advance( uint32_t length );
		virtual void process() = 0;
		void start();
		bool isfinished();

	public:
		FormatTask( Stream* stream );
		~FormatTask();
		FormatTask( FormatTask const& ) = delete;
		FormatTask& operator=( FormatTask const& ) = delete;

		virtual bool poll() override;
	};
}
