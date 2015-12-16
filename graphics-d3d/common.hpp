#ifndef GRAPHICS_COMMON_HPP__
#define GRAPHICS_COMMON_HPP__ 1

#include <utils/strexception.hpp>
#include <windows.h>
#include <cmath>
#include <cinttypes>

namespace graphics
{
	void checkerror_pos(
		char const* filename, char const* function, int line, HRESULT hr );

	inline uint32_t argb8( float const* f )
	{
		float mf[ 4 ];
		for( int i = 0; i < 4; ++i )
		{
			mf[ i ] = ( fabs( f[ i ] ) - fabs( f[ i ] - 1 ) ) * 127.5 + 128;
		}
		uint32_t ri = mf[ 0 ];
		uint32_t gi = mf[ 1 ];
		uint32_t bi = mf[ 2 ];
		uint32_t ai = mf[ 3 ];
		return ai << 24 | ri << 16 | gi << 8 | bi;
	}

	inline int p2align( int x )
	{
		x -= 1;
		int r = 1;
		while( x > 0 ) {
			r <<= 1;
			x >>= 1;
		}
		return r;
	}

	template< typename T >
	inline void RELEASE( T*& ref )
	{
		if( ref )
		{
			ref->Release();
			ref = 0;
		}
	}

	inline int RANGE_ASSERT( int value, int min, int max, char const* name )
	{
		if( value >= min && value < max )
		{
			return value;
		}
		else
		{
			throw utils::StrException( "unknown %s value", name );
		}
	}

	template< typename T >
	inline auto TABLE_ASSERT(
		T& table, int value, char const* name )
		-> decltype( table[ 0 ] )
	{
		return table[ RANGE_ASSERT(
			value,
			0,
			sizeof( table ) / sizeof( table[ 0 ] ),
			name ) ];
	}
}

#define checkerror( hr ) checkerror_pos( __FILE__, __FUNCTION__, __LINE__, hr )

#endif
