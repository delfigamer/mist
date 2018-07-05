#include <utils/cbase.hpp>
#include <utils/console.hpp>

namespace utils
{
	void allocateconsole()
	{
		Console->allocateconsole();
	}

	void write( char const* str )
	{
		externalassert( str );
		Console->write( "%s", str );
	}

	void getchar( char* str )
	{
		externalassert( str );
		Console->getchar( str );
	}
}
