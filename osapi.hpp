#pragma once

#if defined( _WIN32 ) || defined( _WIN64 )
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <utils/encoding.hpp>
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
			throw utils::StrException( "[%73s:%4i]\tunknown Win32 error", file, line );
		}
		wchar_t* wbuffer;
		int buflen = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_ALLOCATE_BUFFER,
			0,
			lasterror,
			MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
			( LPWSTR )&wbuffer,
			0,
			0 );
		if( buflen == 0 )
		{
			throw utils::StrException( "[%73s:%4i]\tWin32 error %i", file, line, lasterror );
		}
		utils::translation_t translation = {
			&utils::encoding::utf16,
			&utils::encoding::utf8,
			wbuffer,
			0,
			0,
			0,
			0xfffd,
		};
		if( utils::translatestr( &translation ) !=
			utils::translateresult::success )
		{
			LocalFree( wbuffer );
			throw utils::StrException( "[%73s:%4i]\tWin32 error %i", file, line, lasterror );
		}
		char* cbuffer = new char[ translation.destresult ];
		translation.dest = cbuffer;
		translation.sourcesize = translation.sourceresult;
		translation.destsize = translation.destresult;
		if( utils::translatestr( &translation ) !=
			utils::translateresult::success )
		{
			LocalFree( wbuffer );
			delete[] cbuffer;
			throw utils::StrException( "[%73s:%4i]\tWin32 error %i", file, line, lasterror );
		}
		utils::StrException e( "[%73s:%4i]\t%s", file, line, cbuffer );
		LocalFree( wbuffer );
		delete[] cbuffer;
		throw e;
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
