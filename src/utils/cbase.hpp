#pragma once

#include <common.hpp>
#include <thread>

namespace utils
{
	[[ r::method ]] inline void yield() noexcept
	{
		std::this_thread::yield();
	}

	[[ r::method ]] void write( char const* str );
	[[ r::method ]] void getchar( char* str );
}
