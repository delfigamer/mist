#pragma once

#include <common/strexception.hpp>
#include <common/string.hpp>
#include <common.hpp>
#include <windows.h>
#include <cmath>
#include <cstdint>

namespace graphics
{
	namespace
	{
		uint32_t argb8( float const* f )
		{
			float mf[ 4 ];
			for( int i = 0; i < 4; ++i )
			{
				mf[ i ] = ( fabs( f[ i ] ) - fabs( f[ i ] - 1 ) ) * 127.5f + 128;
			}
			uint32_t ri = uint32_t( mf[ 0 ] );
			uint32_t gi = uint32_t( mf[ 1 ] );
			uint32_t bi = uint32_t( mf[ 2 ] );
			uint32_t ai = uint32_t( mf[ 3 ] );
			return ai << 24 | ri << 16 | gi << 8 | bi;
		}

		size_t p2align( size_t x )
		{
			if( x == 0 )
			{
				return 1;
			}
			x -= 1;
			size_t r = 1;
			while( x > 0 )
			{
				r <<= 1;
				x >>= 1;
			}
			return r;
		}

		template< typename T, typename U >
		T RANGE_ASSERT( T value, U min, U max, char const* name )
		{
			if( value >= min && value < max )
			{
				return value;
			}
			else
			{
				throw StrException( "invalid %s value", name );
			}
		}

		template< typename T >
		auto TABLE_ASSERT(
			T& table, size_t value, char const* name )
			-> decltype( table[ 0 ] )
		{
			return table[ RANGE_ASSERT(
				value,
				size_t( 0 ),
				sizeof( table ) / sizeof( table[ 0 ] ),
				name ) ];
		}
	}

	namespace Limits
	{
		enum
		{
			AttributeCount = 8,
		};
	}
}
