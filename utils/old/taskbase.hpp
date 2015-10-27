#ifndef UTILS_TASKBASE_HPP__
#define UTILS_TASKBASE_HPP__ 1

#include "refobject.hpp"
#include "ref.hpp"
#include <atomic>
#include <exception>

namespace utils {
	class TaskBase: public RefObject {
	public:
		static int const MinPriority = int( 0xf0000000 );
		static int const MaxPriority = int( 0x10000000 );
		
	private:
		static std::atomic< int > s_order;
		
	private:
		std::atomic< int > const m_order;
		std::atomic< int > m_priority;
		std::atomic< bool > m_poschanged;
		std::atomic< int > m_finished;
		std::atomic< std::exception_ptr > m_error;
		
	public:
		virtual bool query( int id, void** target ) override;
		TaskBase() = delete;
		TaskBase( int priority );
		virtual ~TaskBase() override;
		TaskBase( TaskBase const& other ) = delete;
		TaskBase( TaskBase&& other ) = delete;
		TaskBase& operator=( TaskBase const& other ) = delete;
		TaskBase& operator=( TaskBase&& other ) = delete;
		
		int order() const;
		int priority() const;
		virtual void setpriority( int priority );
		bool poschanged() const;
		void setposchanged( bool poschanged );
		bool finished() const;
		void setfinished( bool finished );
		void error( std::exception_ptr* perror );
		void seterror( std::exception_ptr const& error );
		virtual void flush() = 0;
		virtual void action() = 0;
		
		bool operator<( TaskBase const& other ) const;
		bool operator>( TaskBase const& other ) const;
		bool operator<=( TaskBase const& other ) const;
		bool operator>=( TaskBase const& other ) const;
	};
}

DEFINE_REFID( utils::TaskBase, utils::RefObject, 0x7c16c52d );

namespace utils {
	struct TaskComparer {
		bool operator()( Ref< TaskBase > const& a, Ref< TaskBase > const& b ) { return *a < *b; }
	};
	
	extern "C" {
		int utils_taskbase_order( TaskBase* task ) noexcept;
		int utils_taskbase_priority( TaskBase* task ) noexcept;
		bool utils_taskbase_setpriority( TaskBase* task, int priority ) noexcept;
		int utils_taskbase_finished( TaskBase* task ) noexcept;
		bool utils_taskbase_flush( TaskBase* task ) noexcept;
	}
}

#endif
