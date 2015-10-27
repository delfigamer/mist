#include "winapi.hpp"
#include "strexception.hpp"
#include "encoding.hpp"
#include <stdexcept>

namespace utils
{
	void winerror()
	{
		DWORD lasterror = GetLastError();
		if( !lasterror )
		{
			throw StrException( "unknown Win32 error" );
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
			throw StrException( "Win32 error %i", lasterror );
		}
		translation_t translation = {
			&encoding::utf16,
			&encoding::utf8,
			wbuffer,
			0,
			0,
			0,
			0xfffd,
		};
		if( translatestr( &translation ) != translate_success )
		{
			LocalFree( wbuffer );
			throw StrException( "Win32 error %i", lasterror );
		}
		uint8_t* cbuffer = new uint8_t[ translation.destresult ];
		translation.dest = cbuffer;
		translation.sourcesize = translation.sourceresult;
		translation.destsize = translation.destresult;
		if( translatestr( &translation ) != translate_success )
		{
			delete[] cbuffer;
			LocalFree( wbuffer );
			throw StrException( "Win32 error %i", lasterror );
		}
		StrException exc( "%s", cbuffer );
		delete[] cbuffer;
		LocalFree( wbuffer );
		throw exc;
	}
}
