#pragma once

#include <common.hpp>

namespace rsbin
{
	R_CLASS( name = rsbin )
	struct Common
	{
	public:
		R_METHOD() static bool isfileavailable( char const* path );
	};
}
