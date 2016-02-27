#include "common.hpp"
#include <utils/path.hpp>
#include <utils/ref.hpp>
#include <utils/cbase.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <osapi.hpp>
#else
#include <sys/stat.h>
#endif
#include <stdexcept>

namespace rsbin
{
	bool Common::isfileavailable( char const* path )
	{
		utils::Ref< utils::Path > upath = utils::Path::create( path );
#if defined( _WIN32 ) || defined( _WIN64 )
		return GetFileAttributesW( upath->combine() ) != INVALID_FILE_ATTRIBUTES;
#else
		struct stat64 statbuf;
		int statresult = stat64( upath->combine(), &statbuf );
		return statresult;
#endif
	}

	// uint64_t Common::filesize( char const* path )
	// {
		// struct stat64 statbuf;
		// int statresult = stat64( path, &statbuf );
		// if( statresult )
		// {
			// throw std::runtime_error( "filesize failed" );
		// }
		// else
		// {
			// return statbuf.st_size;
		// }
	// }
}
