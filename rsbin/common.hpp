#ifndef RSBIN_COMMON_HPP__
#define RSBIN_COMMON_HPP__ 1

#include <common.hpp>

namespace rsbin
{
	R_CLASS( name = rsbin )
	class Common
	{
	public:
		R_METHOD() static bool isfileavailable( char const* path );
	};
}

#endif
