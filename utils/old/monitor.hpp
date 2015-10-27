#ifndef UTILS_MONITOR_HPP__
#define UTILS_MONITOR_HPP__ 1

#include "refobject.hpp"
#include <mutex>

namespace utils {
	class Monitor: public RefObject {
	protected:
		typedef std::recursive_mutex mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		mutable mutex_t m_mutex;
		
	public:
		virtual bool query( int id, void** target ) override;
		Monitor();
		Monitor( Monitor const& ) = delete;
		Monitor( Monitor&& ) = delete;
		virtual ~Monitor() override;
		Monitor& operator=( Monitor const& ) = delete;
		Monitor& operator=( Monitor&& ) = delete;
		
		void lock() const;
		void unlock() const;
	};
}

DEFINE_REFID( utils::Monitor, utils::RefObject, 0x3d897ea2 )

namespace utils {
	extern "C" {
		bool utils_monitor_lock( Monitor* m ) noexcept;
		bool utils_monitor_unlock( Monitor* m ) noexcept;
	}
}

#endif
