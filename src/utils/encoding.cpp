#include <utils/encoding.hpp>
#include <stdexcept>

namespace utils
{
	// returns the expected length of the code point, or 0 on error
	// charcode may be 0
	static inline size_t mbfirst( uint8_t ch, uint32_t* charcode )
	{
		uint32_t code;
		size_t result;
		if( ( ch & 0xe0 ) == 0xc0 )
		{
			code = ch & 0x1f;
			result = 2;
		}
		else if( ( ch & 0xf0 ) == 0xe0 )
		{
			code = ch & 0xf;
			result = 3;
		}
		else if( ( ch & 0xf8 ) == 0xf0 )
		{
			code = ch & 7;
			result = 4;
		}
		else
		{
			return 0;
		}
		if( charcode )
		{
			*charcode = code;
		}
		return result;
	}

	static bool utf8_encode(
		void* dest,
		uint32_t charcode,
		size_t destsize,
		size_t* pointlength )
	{
		size_t length;
		if( charcode <= 0x7f )
		{
			length = 1;
		}
		else if( charcode <= 0x7ff )
		{
			length = 2;
		}
		else if( charcode <= 0xd7ff )
		{
			length = 3;
		}
		else if( charcode <= 0xdfff )
		{
			*pointlength = 0;
			return false;
		}
		else if( charcode <= 0xffff )
		{
			length = 3;
		}
		else if( charcode <= 0x10ffff )
		{
			length = 4;
		}
		else
		{
			*pointlength = 0;
			return false;
		}
		*pointlength = length;
		if( dest )
		{
			if( destsize >= length )
			{
				uint8_t* destw = ( uint8_t* )dest;
				if( length == 1 )
				{
					destw[ 0 ] = charcode;
				}
				else if( length == 2 )
				{
					destw[ 0 ] = 0xc0 | ( 0x1f & charcode >> 6 );
					destw[ 1 ] = 0x80 | ( 0x3f & charcode );
				}
				else if( length == 3 )
				{
					destw[ 0 ] = 0xe0 | ( 0xf & charcode >> 12 );
					destw[ 1 ] = 0x80 | ( 0x3f & charcode >> 6 );
					destw[ 2 ] = 0x80 | ( 0x3f & charcode );
				}
				else if( length == 4 )
				{
					destw[ 0 ] = 0xf0 | ( 7 & charcode >> 18 );
					destw[ 1 ] = 0x80 | ( 0x3f & charcode >> 12 );
					destw[ 2 ] = 0x80 | ( 0x3f & charcode >> 6 );
					destw[ 3 ] = 0x80 | ( 0x3f & charcode );
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return destsize == 0 || destsize >= length;
		}
		return false;
	}

	static bool utf8_decode(
		void const* source,
		uint32_t* charcode,
		size_t sourcesize,
		size_t* pointlength )
	{
		uint8_t const* sourcew = ( uint8_t const* )source;
		if( ( sourcew[ 0 ] & 0x80 ) == 0 )
		{
			if( charcode )
			{
				*charcode = sourcew[ 0 ];
			}
			*pointlength = 1;
			return true;
		}
		uint32_t code;
		size_t length = mbfirst( sourcew[ 0 ], &code );
		if( length == 0 )
		{
			*pointlength = 1;
			return false;
		}
		else if( sourcesize != 0 && sourcesize < length )
		{
			*pointlength = 0;
			return false;
		}
		for( size_t i = 1; i < length; ++i )
		{
			if( ( sourcew[ i ] & 0xc0 ) == 0x80 )
			{
				code = code << 6 | ( 0x3f & sourcew[ i ] );
			}
			else
			{
				*pointlength = i;
				return false;
			}
		}
		*pointlength = length;
		if( ( code >= 0xd800 && code <= 0xdfff ) || code > 0x10ffff )
		{
			return false;
		}
		if( charcode )
		{
			*charcode = code;
		}
		return true;
	}

	static bool utf16_encode(
		void* dest,
		uint32_t charcode,
		size_t destsize,
		size_t* pointlength )
	{
		if( charcode <= 0xd7ff || ( charcode >= 0xe000 && charcode <= 0xffff ) )
		{
			*pointlength = 2;
			if( dest )
			{
				if( destsize >= 2 )
				{
					uint16_t* destw = ( uint16_t* )dest;
					destw[ 0 ] = charcode;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return destsize != 1; // destsize == 0 || destsize >= 2;
			}
		}
		else if( charcode >= 0x10000 && charcode <= 0x10ffff )
		{
			*pointlength = 4;
			if( dest )
			{
				if( destsize >= 4 )
				{
					uint16_t* destw = ( uint16_t* )dest;
					uint32_t value = charcode - 0x10000;
					destw[ 0 ] = ( value >> 10 ) | 0xd800;
					destw[ 1 ] = ( value & 0x3ff ) | 0xdc00;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return destsize == 0 || destsize >= 4;
			}
		}
		else
		{
			*pointlength = 0;
			return false;
		}
	}

	static bool utf16_decode(
		void const* source,
		uint32_t* charcode,
		size_t sourcesize,
		size_t* pointlength )
	{
		if( sourcesize == 1 ) // sourcesize != 0 && sourcesize < 2
		{
			*pointlength = 0;
			return false;
		}
		uint16_t const* sourcew = ( uint16_t const* )source;
		uint16_t lead = sourcew[ 0 ];
		if( lead >= 0xdc00 && lead <= 0xdfff )
		{
			*pointlength = 2;
			return false;
		}
		else if( lead >= 0xd800 && lead <= 0xdbff )
		{
			if( sourcesize != 0 && sourcesize < 4 )
			{
				*pointlength = 0;
				return false;
			}
			uint16_t low = sourcew[ 1 ];
			if( low >= 0xdc00 && low <= 0xdfff )
			{
				if( charcode )
				{
					low &= 0x3ff;
					uint32_t high = uint32_t( lead & 0x3ff ) << 10;
					*charcode = ( high | low ) + 0x10000;
				}
				*pointlength = 4;
				return true;
			}
			else
			{
				*pointlength = 2;
				return false;
			}
		}
		else
		{
			if( charcode )
			{
				*charcode = lead;
			}
			*pointlength = 2;
			return true;
		}
	}

	// static bool utf32_encode(
		// void* dest,
		// uint32_t charcode,
		// size_t destsize,
		// size_t* pointlength )
	// {
		// if( ( charcode >= 0xd800 && charcode <= 0xdfff ) || charcode > 0x10ffff )
		// {
			// *pointlength = 0;
			// return false;
		// }
		// else
		// {
			// *pointlength = 4;
			// if( dest )
			// {
				// if( destsize >= 4 )
				// {
					// uint32_t* destw = ( uint32_t* )dest;
					// destw[ 0 ] = charcode;
					// return true;
				// }
				// else
				// {
					// return false;
				// }
			// }
			// else
			// {
				// return destsize == 0 || destsize >= 4;
			// }
		// }
	// }

	// static bool utf32_decode(
		// void const* source,
		// uint32_t* charcode,
		// size_t sourcesize,
		// size_t* pointlength )
	// {
		// if( sourcesize != 0 && sourcesize < 4 )
		// {
			// *pointlength = 0;
			// return false;
		// }
		// *pointlength = 4;
		// uint32_t const* sourcew = ( uint32_t const* )source;
		// uint32_t code = sourcew[ 0 ];
		// *pointlength = 4;
		// if( ( code >= 0xd800 && code <= 0xdfff ) || code > 0x10ffff )
		// {
			// return false;
		// }
		// if( charcode )
		// {
			// *charcode = code;
		// }
		// return true;
	// }

	// template< int bl, int bh >
	// static bool utf16xe_encode(
		// void* dest,
		// uint32_t charcode,
		// size_t destsize,
		// size_t* pointlength )
	// {
		// if( dest )
		// {
			// uint16_t buffer[ 2 ];
			// size_t length;
			// bool success = utf16_encode( buffer, charcode, destsize, &length );
			// if( success )
			// {
				// uint8_t* destw = ( uint8_t* )dest;
				// destw[ bl ] = uint8_t( buffer[ 0 ] );
				// destw[ bh ] = uint8_t( buffer[ 0 ] >> 8 );
				// if( length == 4 )
				// {
					// destw[ 2 + bl ] = uint8_t( buffer[ 1 ] );
					// destw[ 2 + bh ] = uint8_t( buffer[ 1 ] >> 8 );
				// }
			// }
			// *pointlength = length;
			// return success;
		// }
		// else
		// {
			// return utf16_encode( 0, charcode, destsize, pointlength );
		// }
	// }

	// template< int bl, int bh >
	// static bool utf16xe_decode(
		// void const* source,
		// uint32_t* charcode,
		// size_t sourcesize,
		// size_t* pointlength )
	// {
		// uint8_t const* sourcew = ( uint8_t const* )source;
		// uint16_t buffer[ 2 ];
		// if( sourcesize >= 2 )
		// {
			// buffer[ 0 ] =
				// uint16_t( sourcew[ bl ] ) | uint16_t( sourcew[ bh ] << 8 );
			// if( buffer[ 0 ] >= 0xd800 && buffer[ 0 ] <= 0xdbff && sourcesize >= 4 )
			// {
				// buffer[ 1 ] =
					// uint16_t( sourcew[ 2 + bl ] )
					// | uint16_t( sourcew[ 2 + bh ] << 8 );
			// }
		// }
		// return utf16_decode( buffer, charcode, sourcesize, pointlength );
	// }

	// template< int b1, int b2, int b3, int b4 >
	// static bool utf32xe_encode(
		// void* dest,
		// uint32_t charcode,
		// size_t destsize,
		// size_t* pointlength )
	// {
		// if( dest )
		// {
			// uint32_t buffer[ 1 ];
			// bool success = utf32_encode( buffer, charcode, destsize, pointlength );
			// if( success )
			// {
				// uint8_t* destw = ( uint8_t* )dest;
				// destw[ b1 ] = buffer[ 0 ];
				// destw[ b2 ] = buffer[ 0 ] >> 8;
				// destw[ b3 ] = buffer[ 0 ] >> 16;
				// destw[ b4 ] = buffer[ 0 ] >> 24;
			// }
			// return success;
		// }
		// else
		// {
			// return utf32_encode( 0, charcode, destsize, pointlength );
		// }
	// }

	// template< int b1, int b2, int b3, int b4 >
	// static bool utf32xe_decode(
		// void const* source,
		// uint32_t* charcode,
		// size_t sourcesize,
		// size_t* pointlength )
	// {
		// uint8_t const* sourcew = ( uint8_t const* )source;
		// uint32_t buffer[ 1 ];
		// if( sourcesize >= 4 )
		// {
			// buffer[ 0 ] =
				// uint32_t( sourcew[ b1 ] ) |
				// uint32_t( sourcew[ b2 ] << 8 ) |
				// uint32_t( sourcew[ b3 ] << 16 ) |
				// uint32_t( sourcew[ b4 ] << 24 );
			// return utf32_decode( buffer, charcode, sourcesize, pointlength );
		// }
		// else
		// {
			// return utf32_decode( 0, charcode, sourcesize, pointlength );
		// }
	// }

	namespace encoding
	{
		encoding_t const utf8{ &utf8_encode, &utf8_decode };
		encoding_t const utf16{ &utf16_encode, &utf16_decode };
		// encoding_t const utf32{ &utf32_encode, &utf32_decode };
		// encoding_t const utf16le{
			// &utf16xe_encode< 0, 1 >, &utf16xe_decode< 0, 1 > };
		// encoding_t const utf32le{
			// &utf32xe_encode< 0, 1, 2, 3 >, &utf32xe_decode< 0, 1, 2, 3 > };
		// encoding_t const utf16be{
			// &utf16xe_encode< 1, 0 >, &utf16xe_decode< 1, 0 > };
		// encoding_t const utf32be{
			// &utf32xe_encode< 3, 2, 1, 0 >, &utf32xe_decode< 3, 2, 1, 0 > };
	}

	static size_t const sourcesize_zterm = size_t( -1 );

	translateresult translatestr(
		translation_t& translation )
	{
		uint8_t const* source = ( uint8_t const* )translation.source;
		uint8_t* dest = ( uint8_t* )translation.dest;
		size_t sourcesize = translation.sourcesize;
		if( sourcesize == 0 )
		{
			sourcesize = sourcesize_zterm;
		}
		size_t destsize = translation.destsize;
		uint32_t defaultchar = translation.defaultchar;
		size_t sourceresult = 0;
		size_t destresult = 0;
		translateresult resultcode = translateresult::success;
		while( sourcesize != 0 )
		{
			uint32_t charcode;
			size_t spointlength;
			if( !translation.senc->decode(
				source,
				&charcode,
				sourcesize == sourcesize_zterm ? 0 : sourcesize, &spointlength ) )
			{
				if( spointlength == 0 )
				{
					resultcode = translateresult::source_overrun;
					break;
				}
				else
				{
					if( defaultchar == 0 )
					{
						resultcode = translateresult::source_corrupted;
						break;
					}
					else
					{
						charcode = defaultchar;
					}
				}
			}
			source += spointlength;
			if( sourcesize != sourcesize_zterm )
			{
				sourcesize -= spointlength;
			}
			size_t dpointlength;
			if( !translation.denc->encode(
				dest, charcode, destsize, &dpointlength ) )
			{
				if( dpointlength == 0 )
				{
					resultcode = translateresult::dest_unsupported;
					break;
				}
				else
				{
					resultcode = translateresult::dest_overrun;
					break;
				}
			}
			if( dest )
			{
				dest += dpointlength;
			}
			if( destsize != 0 )
			{
				destsize -= dpointlength;
			}
			sourceresult += spointlength;
			destresult += dpointlength;
			if( sourcesize == sourcesize_zterm && charcode == 0 )
			{
				break;
			}
		}
		translation.sourceresult = sourceresult;
		translation.destresult = destresult;
		return resultcode;
	}

	Ref< DataBuffer > translatebuffer(
		encoding_t const* senc, encoding_t const* denc,
		void const* source, size_t sourcesize )
	{
		translation_t translation = {
			senc,
			denc,
			source,
			0,
			sourcesize,
			0,
			0xfffd,
		};
		translatestr( translation );
		Ref< DataBuffer > db = DataBuffer::create( translation.destresult );
		translation.dest = db->m_data;
		translation.sourcesize = translation.sourceresult;
		translation.destsize = db->m_capacity;
		utils::translatestr( translation );
		return db;
	}

	Ref< DataBuffer > translatebuffer(
		encoding_t const* senc, encoding_t const* denc, DataBuffer* source )
	{
		return translatebuffer( senc, denc, source->m_data, source->m_length );
	}
}
