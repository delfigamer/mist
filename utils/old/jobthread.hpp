#ifndef UTILS_JOBTHREAD_HPP__
#define UTILS_JOBTHREAD_HPP__ 1

#include <thread>
#include <mutex>
#include <exception>
#include <queue>
#include <tuple>

namespace utils {
	struct Job {
		void* data;
		void( *action )( void* );
		void( *destroy )( void* );
		void( *error )( void*, std::exception_ptr&& eptr );
		
		Job() = default;
		Job(
			void* _data,
			void( *_action )( void* ),
			void( *_destroy )( void* ) = 0,
			void( *_error )( void*, std::exception_ptr&& eptr ) = 0 );
	};
	
	class JobThread {
	private:
		std::thread m_thread;
		std::mutex m_mutex;
		std::queue< Job > m_queue;
		Job m_idle;
		bool m_terminate;
		
		void threadfunc() noexcept;
		bool pop( Job* job ) noexcept;
		void next( Job* job ) noexcept;
		static void invoke( Job const& job ) noexcept;
		static void notify( Job const& job, std::exception_ptr&& eptr ) noexcept;
		static void finalize( Job const& job ) noexcept;
		static void yield( void* );
		
	public:
		JobThread();
		~JobThread();
		JobThread( const JobThread& other ) = delete;
		JobThread( JobThread&& other ) = delete;
		JobThread& operator=( const JobThread& other ) = delete;
		JobThread& operator=( JobThread&& other ) = delete;
		
		void push(
			void* data,
			void( *action )( void* ),
			void( *destroy )( void* ) = 0,
			void( *error )( void*, std::exception_ptr&& eptr ) = 0 );
		void push( Job const& job );
		void setidle( Job const& job );
		void setidle();
		bool empty();
	};
}

#endif
