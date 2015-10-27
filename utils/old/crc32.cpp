#include "crc32.hpp"

namespace utils {
	struct crc32_lookup {
		static const uint32_t Polynomial = 0x82F63B78;
		// CRC-32C
		uint32_t b[ 256 ];
		crc32_lookup() {
			uint32_t i;
			uint32_t j;
			uint32_t crc;
			for( i = 0; i <= 0xFF; i++ ) {
				crc = i;
				for( j = 0; j < 8; j++ ) {
					crc = ( crc >> 1 ) ^ ( - int32_t( crc & 1 ) & Polynomial );
				}
				b[ i ] = crc;
			}
		}
	};
	
	uint32_t crc32( const void* data, int length, uint32_t start ) {
		static const crc32_lookup lookup;
		uint32_t crc = uint32_t( ~start );
		uint8_t const* current = ( uint8_t const* )data;
		while( length-- ) {
			crc = ( crc >> 8 ) ^ lookup.b[ ( crc & 0xFF ) ^ *current++ ];
		}
		return ~crc;
	}
}
