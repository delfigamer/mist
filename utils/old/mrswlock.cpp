#include "mrswlock.hpp"
#include <thread>

namespace utils {
	MRSWLock::MRSWLock() {
	}
	
	MRSWLock::~MRSWLock() {
	}
	
	void MRSWLock::rlock() {
		while( m_wflag.load( std::memory_order_seq_cst ) ) {
			std::this_thread_yield();
		}
	}
	
	void MRSWLock::rrelease();
	void MRSWLock::wlock();
	void MRSWLock::wrelease();
}

#endif
