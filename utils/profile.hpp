#ifndef UTILS_PROFILE_HPP__
#define UTILS_PROFILE_HPP__ 1

#include <common.hpp>
#include <atomic>
#include <time.h>
#include <ctime>

#define UTILS_PROFILE_STRINGIFY_INNER( x ) #x
#define UTILS_PROFILE_STRINGIFY( x ) UTILS_PROFILE_STRINGIFY_INNER( x )
#define UTILS_PROFILE_CONCAT_INNER( a, b ) a ## b
#define UTILS_PROFILE_CONCAT( a, b ) UTILS_PROFILE_CONCAT_INNER( a, b )
#define PROFILE( name, ... ) { \
	static utils::Profiler UTILS_PROFILE_CONCAT( __profiler_, __LINE__ )( \
		__FILE__ ":" UTILS_PROFILE_STRINGIFY( __LINE__ ) "( " name " )" ); \
	utils::ProfileTimer UTILS_PROFILE_CONCAT( __profiletimer_, __LINE__ )( \
		&UTILS_PROFILE_CONCAT( __profiler_, __LINE__ ) ); \
	{ __VA_ARGS__ } \
}

namespace utils
{
	class Profiler
	{
	private:
		std::atomic_flag m_aflag;
		uint64_t m_time;
		uint32_t m_min;
		uint32_t m_max;
		int m_count;
		char const* m_name;

	public:
		Profiler() = delete;
		Profiler( char const* name );
		~Profiler();
		Profiler( Profiler const& ) = delete;
		Profiler& operator=( Profiler const& ) = delete;

		void sample( uint32_t ns );
	};

	class ProfileTimer
	{
	private:
		Profiler* m_profiler;
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( __MACHINEX86 )
		// On x86 and x86-64 CPUs we use RDTSC
		uint64_t m_start;
#else
		timespec m_start;
#endif

	public:
		ProfileTimer() = delete;
		ProfileTimer( Profiler* profiler );
		~ProfileTimer();
		ProfileTimer( ProfileTimer const& ) = delete;
		ProfileTimer operator=( ProfileTimer const& ) = delete;
	};

	inline void Profiler::sample( uint32_t time )
	{
		while( m_aflag.test_and_set() )
		{
		}
		m_time += time;
		if( time < m_min )
		{
			m_min = time;
		}
		if( time > m_max )
		{
			m_max = time;
		}
		m_count += 1;
		m_aflag.clear();
	}

#if defined( __i386__ ) || defined( __x86_64__ ) || defined( __MACHINEX86 )
	inline uint64_t RDTSC()
	{
#if defined( _MSC_VER )
		return __rdtsc();
#else
		uint32_t hi, lo;
		__asm__ volatile( "rdtsc" : "=a"( lo ), "=d"( hi ) );
		return ( uint64_t( hi ) << 32 ) | lo;
#endif
	}

	inline uint64_t current_time()
	{
		static uint32_t factor = 0;
		if( factor == 0 )
		{
			clock_t cv = clock() + CLOCKS_PER_SEC;
			uint64_t base = RDTSC();
			while( clock() < cv ) {
			}
			uint64_t ticks_per_s = RDTSC() - base;
			factor = uint32_t( ( 1000000000LL << 16 ) / ticks_per_s );
		}
		return ( RDTSC() * factor ) >> 16;
	}

	inline ProfileTimer::ProfileTimer( Profiler* profiler )
		: m_profiler( profiler )
	{
		m_start = current_time();
	}

	inline ProfileTimer::~ProfileTimer()
	{
		m_profiler->sample( uint32_t( current_time() - m_start ) );
	}
#else
	inline ProfileTimer::ProfileTimer( Profiler* profiler )
		: m_profiler( profiler )
	{
		clock_gettime( CLOCK_MONOTONIC, &m_start );
	}

	inline ProfileTimer::~ProfileTimer()
	{
		timespec ts;
		clock_gettime( CLOCK_MONOTONIC, &ts );
		int32_t dt = ts.tv_nsec - m_start.tv_nsec +
			1000000000 * (ts.tv_sec - m_start.tv_sec);
		m_profiler->sample( dt );
	}
#endif
}

#endif
