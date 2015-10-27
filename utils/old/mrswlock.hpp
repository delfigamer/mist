#ifndef UTILS_MRSWLOCK_HPP__
#define UTILS_MRSWLOCK_HPP__ 1

#include <atomic>

namespace utils {
	class MRSWLock {
	private:
		std::atomic_flag m_flag;
		int m_rcount;
		bool m_wflag;
		
	public:
		MRSWLock();
		~MRSWLock();
		MRSWLock( MRSWLock const& other ) = delete;
		MRSWLock( MRSWLock&& other ) = delete;
		MRSWLock& operator=( MRSWLock const& other ) = delete;
		MRSWLock& operator=( MRSWLock&& other ) = delete;
		
		void rlock();
		void rrelease();
		void wlock();
		void wrelease();
	};
}

#endif
