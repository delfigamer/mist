#pragma once

#include <common.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#if !defined( WIN32_LEAN_AND_MEAN )
#define WIN32_LEAN_AND_MEAN
#endif
// The minimum target is Windows 7
#if defined( WINVER )
#undef WINVER
#endif
#define WINVER 0x0601
#if defined( _WIN32_WINNT )
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0601
#include <windows.h>
#elif defined( __ANDROID__ )
#include <cerrno>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <stdexcept>
#include <string>
#include <cstdio>

namespace
{
[[maybe_unused, noreturn]]
	void syserrorexpos( int err, char const* file, int line );
[[maybe_unused, noreturn]]
	void syserrorpos( char const* file, int line );

	void syserrorexpos( int err, char const* file, int line )
	{
		char buffer[ 1024 ];
#if defined( _WIN32 ) || defined( _WIN64 )
		if( err == 0 )
		{
#if defined( MIST_DEBUG )
			snprintf( buffer, sizeof( buffer ),
				"[%73s:%4i]\tunknown Win32 error", file, line );
#else
			snprintf( buffer, sizeof( buffer ),
				"unknown Win32 error" );
#endif
		}
		else
		{
			char fmbuffer[ 1024 ];
			size_t fmbuflen = FormatMessageA(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
				0,
				( DWORD )err,
				MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
				buffer,
				sizeof( fmbuffer ) - 1,
				0 );
			if( fmbuflen == 0 )
			{
#if defined( MIST_DEBUG )
				snprintf( buffer, sizeof( buffer ),
					"[%73s:%4i]\tWin32 error %#x", file, line, err );
#else
				snprintf( buffer, sizeof( buffer ),
					"Win32 error %#x", err );
#endif
			}
#if defined( MIST_DEBUG )
			snprintf( buffer, sizeof( buffer ),
				"[%73s:%4i]\t%s", file, line, fmbuffer );
#else
			snprintf( buffer, sizeof( buffer ),
				"%s", fmbuffer );
#endif
		}
#elif defined ( __ANDROID__ )
#if defined( MIST_DEBUG )
		snprintf( buffer, sizeof( buffer ),
			"[%73s:%4i]\t%s", file, line, strerror( err ) );
#else
		snprintf( buffer, sizeof( buffer ),
			"%s", strerror( err ) );
#endif
#endif
		throw std::runtime_error( std::string( buffer ) );
	}

	void syserrorpos( char const* file, int line )
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		syserrorexpos( ( int )GetLastError(), file, line );
#elif defined ( __ANDROID__ )
		syserrorexpos( errno, file, line );
#endif
	}
}

#define syserror() syserrorpos( __FILE__, __LINE__ )
#define syserrorex( err ) syserrorexpos( err, __FILE__, __LINE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
#define PATH_START "..\\"
#define PATH_SEP "\\"
#define PATH_SEP_DB "\\\0"_db
#elif defined( __ANDROID__ )
#define PATH_START "/storage/sdcard0/Android/data/me.sheimi.sgit/files/repo/mist/output/"
#define PATH_SEP "/"
#endif
