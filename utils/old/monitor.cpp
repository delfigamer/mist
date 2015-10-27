#include "monitor.hpp"
#include "cbase.hpp"

namespace utils {
	bool Monitor::query( int id, void** target ) {
		return queryobject( this, id, target );
	}
	
	Monitor::Monitor() {
	}
	
	Monitor::~Monitor() {
	}
	
	void Monitor::lock() const {
		m_mutex.lock();
	}
	
	void Monitor::unlock() const {
		m_mutex.unlock();
	}
	
	extern "C" {
		bool utils_monitor_lock( Monitor* m ) noexcept {
			CBASE_PROTECT(
				m->lock();
				return 1;
			)
		}
		
		bool utils_monitor_unlock( Monitor* m ) noexcept {
			CBASE_PROTECT(
				m->unlock();
				return 1;
			)
		}
	}
}
