#include "profile.hpp"
#include "console.hpp"
#include <cstdio>

namespace utils
{
	Profiler::Profiler( char const* name )
		: m_time( 0 )
		, m_min( uint32_t( 0xffffffff ) )
		, m_max( 0 )
		, m_count( 0 )
		, m_name( name )
	{
		m_aflag.clear();
	}

	Profiler::~Profiler()
	{
		if( m_count > 0 )
		{
			int avg = int32_t( ( ( m_time << 1 ) + m_count ) / ( m_count << 1 ) );
			Console->writeln(
				"[%78s]\tavg: %4i.%.6ims, min: %4i.%.6ims, max: %4i.%.6ims (%i %s)",
				m_name,
				avg / 1000000, avg % 1000000,
				m_min / 1000000, m_min % 1000000,
				m_max / 1000000, m_min % 1000000,
				m_count, ( m_count == 1 ? "sample" : "samples" ) );
		}
		else
		{
			Console->writeln( "[%78s]\tprofile data never sampled", m_name );
		}
	}
#ifdef __i386__
	static int dummy_value = current_time();
#endif
}
