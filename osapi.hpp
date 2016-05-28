#pragma once

#if defined( _WIN32 ) || defined( _WIN64 )
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#elif defined( __ANDROID__ )
#include <cerrno>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <common.hpp>
#include <utils/strexception.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>

namespace
{
	void syserror( char const* file, int line ) ATTRIBUTE(( noreturn ));

	void syserror( char const* file, int line )
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		DWORD lasterror = GetLastError();
		if( !lasterror )
		{
			throw utils::StrException(
				"[%73s:%4i]\tunknown Win32 error", file, line );
		}
		char buffer[ 1024 ];
		size_t buflen = FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
			0,
			lasterror,
			MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
			buffer,
			sizeof( buffer ) - 1,
			0 );
		if( buflen == 0 )
		{
			throw utils::StrException(
				"[%73s:%4i]\tWin32 error %i", file, line, lasterror );
		}
		throw utils::StrException( "[%73s:%4i]\t%s", file, line, buffer );
#elif defined ( __ANDROID__ )
		throw utils::StrException( "[%73s:%4i]\t%s", file, line, strerror( errno ) );
#endif
	}
}

#define syserror() syserror( __FILE__, __LINE__ )
#if defined( _WIN32 ) || defined( _WIN64 )
#define PATH_START "..\\"
#define PATH_SEP "\\"
#define PATH_START_W L"..\\"
#elif defined( __ANDROID__ )
#define PATH_START "/storage/sdcard0/Android/data/me.sheimi.sgit/files/repo/mist/output/"
#define PATH_SEP "/"
#endif
