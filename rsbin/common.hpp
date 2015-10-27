#ifndef RSBIN_COMMON_HPP__
#define RSBIN_COMMON_HPP__ 1

#include <cinttypes>

namespace rsbin {
	extern "C" {
		int rsbin_fileexists( char const* path ) noexcept;
		bool rsbin_filesize(
			char const* path, uint64_t* size ) noexcept;
	}
}

#endif
