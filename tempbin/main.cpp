#include <utils/path.hpp>
#include <utils/ref.hpp>
#include <osapi.hpp>
#include <cstdio>

int main( int argc, char const* const* argv )
{
	utils::Ref< utils::Path > upath = utils::Path::create( argv[ 1 ] );
	fprintf( stderr, "%ls\n", upath->combine() );
	bool r = GetFileAttributesW( upath->combine() ) != INVALID_FILE_ATTRIBUTES;
	fprintf( stderr, "%s\n", r ? "available" : "not available" );
	return 0;
}
