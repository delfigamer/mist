#pragma once

#define r_emit( ... )

#if defined( _MSC_VER )
#define __attribute__( ... )
#define _CRT_SECURE_NO_WARNINGS
#else
#endif

#define externalassert( expr ) \
	( ( expr ) ? \
		( void )0 : \
		throw std::runtime_error( "assertion failed for " #expr ) )

#include <cinttypes>
#include <cassert>

// #include <atomic>
// #include <cstdio>
template< typename T >
struct Counter
{
	static void update( ptrdiff_t delta )
	{
		// static std::atomic< ptrdiff_t > count( ( ptrdiff_t )0 );
		// int value = count.fetch_add(
			// delta, std::memory_order_relaxed ) + delta;
		// fprintf( stderr, "\t\tcount: %i\n", value );
	}
};
