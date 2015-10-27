#ifndef UTILS_CRC32_HPP_
#define UTILS_CRC32_HPP_ 1

#include <cstdint>

namespace utils {
	extern uint32_t crc32( const void* data, int length, uint32_t start = 0 );
}

#endif
