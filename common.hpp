#pragma once

#define R_ENUM( ... )
#define R_STRUCT( ... )
#define R_CLASS( ... )
#define R_EXTERNAL( ... )
#define R_METHOD( ... )
#define R_TYPE( ... )
#define R_EMIT( ... )
#define R_END( ... )

#if defined( _MSC_VER )
#define NOEXCEPT
#define ATTRIBUTE( ... )
#define _CRT_SECURE_NO_WARNINGS
#else
#define NOEXCEPT noexcept
#define ATTRIBUTE __attribute__
#endif

#include <cstdio>
#include <cstdlib>
inline void ASSERT_POS(
	char const* file, int line, bool cond, char const* msg = "assertion failed" )
{
	if( !cond )
	{
		fprintf( stderr, "[%73s:%4i]\t%s\n", file, line, msg );
		abort();
	}
}
#define ASSERT( ... ) ASSERT_POS( __FILE__, __LINE__, ##__VA_ARGS__ )

#if defined( _MSC_VER )
extern "C" __declspec( dllimport ) int __cdecl vsnprintf( char*, size_t, char const*, char* );
// a dirty hack, intended to avoid the inclusion of cstdarg and cstdio
// we rely on the fact that va_list is char* to the position on the stack
namespace
{
	int __cdecl snprintf( char* buf, size_t bufsize, char const* format, ... )
	{
		char* vaargs = ( char* )&format + sizeof( void* );
		int ret = vsnprintf( buf, bufsize, format, vaargs );
		return ret;
	}
}
#endif
