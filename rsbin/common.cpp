#include "common.hpp"
#include <utils/cbase.hpp>
#include <sys/stat.h>
#include <stdexcept>

namespace rsbin
{
	int rsbin_fileexists( char const* path ) noexcept
	{
	CBASE_PROTECT(
		struct stat64 statbuf;
		int statresult = stat64( path, &statbuf );
		return statresult ? 2 : 1;
	)
	}

	bool rsbin_filesize( char const* path, uint64_t* size ) noexcept
	{
	CBASE_PROTECT(
		struct stat64 statbuf;
		int statresult = stat64( path, &statbuf );
		if( statresult )
		{
			throw std::runtime_error( "filesize failed" );
		}
		else
		{
			*size = statbuf.st_size;
			return 1;
		}
	)
	}
}
