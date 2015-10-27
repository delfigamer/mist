#ifndef GRAPHICS_COMMON_HPP__
#define GRAPHICS_COMMON_HPP__ 1

#include <windows.h>
#include <cinttypes>

namespace graphics {
	void checkerror_pos( char const* filename, char const* function, int line, HRESULT hr );
	
	inline uint32_t argb8( float const* f ) {
		float rf = ( f[ 0 ] > 1 ? 1 : f[ 0 ] < 0 ? 0 : f[ 0 ] );
		float gf = ( f[ 1 ] > 1 ? 1 : f[ 1 ] < 0 ? 0 : f[ 1 ] );
		float bf = ( f[ 2 ] > 1 ? 1 : f[ 2 ] < 0 ? 0 : f[ 2 ] );
		float af = ( f[ 3 ] > 1 ? 1 : f[ 3 ] < 0 ? 0 : f[ 3 ] );
		uint32_t ri = rf * 255 + 0.5;
		uint32_t gi = gf * 255 + 0.5;
		uint32_t bi = bf * 255 + 0.5;
		uint32_t ai = af * 255 + 0.5;
		return ai << 24 | ri << 16 | gi << 8 | bi;
	}
}

#define checkerror( hr ) checkerror_pos( __FILE__, __FUNCTION__, __LINE__, hr )

#define RELEASE( ref ) ( void )( ref ? ( ref->Release(), ref = 0 ) : 0 )
#define RANGE_ASSERT( value, min, max, name ) ( \
	( value ) >= int( min ) && ( value ) < int( max ) ? \
		value : throw std::runtime_error( "unknown " name " value" ) )
#define TABLE_ASSERT( table, value, name ) ( table[ \
		RANGE_ASSERT( \
			value, \
			0, \
			sizeof( table ) / sizeof( ( table )[ 0 ] ), \
			name ) ] )

#endif
