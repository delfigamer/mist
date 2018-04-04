#include <utils/cbase.hpp>
#include <utils/console.hpp>

namespace utils
{
	void write( char const* str )
	{
		Console->write( "%s", str );
	}

	void getchar( char* str )
	{
		Console->getchar( str );
	}
}
