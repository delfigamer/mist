#ifndef UTILS_CBASE_HPP__
#define UTILS_CBASE_HPP__ 1

#include <common.hpp>

namespace utils
{
	R_CLASS( name = cbase )
	struct cbase
	{
		R_METHOD( stringwrap ) static char const* geterror() NOEXCEPT;
		R_METHOD() static void seterror( char const* error ) NOEXCEPT;
		R_METHOD() static void yield() NOEXCEPT;
		R_METHOD() static void write( char const* str );
		R_METHOD() static void getchar( char* str );
	};
}

#endif
