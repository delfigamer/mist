#include <rsbin/common.hpp>
#include <utils/path.hpp>
#include <common/ref.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <osapi.hpp>
#else
#include <sys/stat.h>
#endif
#include <stdexcept>

namespace rsbin
{
	bool isfileavailable( char const* path )
	{
		externalassert( path );
		Ref< utils::Path > upath = utils::Path::create( path );
		if( upath )
		{
#if defined( _WIN32 ) || defined( _WIN64 )
			return
				GetFileAttributesW( upath->combine() ) != INVALID_FILE_ATTRIBUTES;
#else
			struct stat64 statbuf;
			int statresult = stat64( upath->combine(), &statbuf );
			return ( bool )statresult;
#endif
		}
		else
		{
			return false;
		}
	}
}
