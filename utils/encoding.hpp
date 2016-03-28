#ifndef UTILS_ENCODING_HPP__
#define UTILS_ENCODING_HPP__ 1

#include <common.hpp>
#include <cinttypes>

namespace utils
{
	// on success - return true, *pointlength <- number of bytes written
	// invalid stream - return false, *pointlength <- 0
	// buffer overrun - return false, *pointlength <- number of bytes that would be written
	// dest may be 0
	// if destsize == 0 && dest == 0, always succeeds
	// if destsize == 0 && dest != 0, always fails
R_EMIT( target = ffi_start )
	typedef bool( *encoder_t )(
		void* dest,
		uint32_t charcode,
		size_t destsize,
		size_t* pointlength );
R_END()

	// on success - return true, *pointlength <- number of bytes consumed
	// invalid stream - return false, *pointlength <- number of bytes consumed
	// buffer overrun - return false, *pointlength <- 0
	// charcode may be 0
	// if sourcesize == 0, source is assumed to be unbound
R_EMIT( target = ffi_start )
	typedef bool( *decoder_t )(
		void const* source,
		uint32_t* charcode,
		size_t sourcesize,
		size_t* pointlength );
R_END()

	R_CLASS()
	struct encoding_t
	{
	R_STRUCT()
		encoder_t encode;
		decoder_t decode;
	R_END()
		encoding_t( encoder_t _encode, decoder_t _decode )
			: encode( _encode )
			, decode( _decode )
		{
		}
	};

	namespace encoding
	{
		extern encoding_t const utf8;
		extern encoding_t const utf16;
		extern encoding_t const utf32;
		extern encoding_t const utf16le;
		extern encoding_t const utf16be;
		extern encoding_t const utf32le;
		extern encoding_t const utf32be;

		R_ENUM( name = encodingindex )
		namespace index
		{
			enum
			{
				utf8 = 0,
				utf16 = 1,
				utf32 = 2,
				utf16le = 3,
				utf16be = 4,
				utf32le = 5,
				utf32be = 6,
			};
		}
	}

	R_CLASS()
	struct translation_t
	{
	R_STRUCT()
		encoding_t const* senc;
		encoding_t const* denc;
		void const* source;
		void* dest; // may be 0
		size_t sourcesize; // if 0, source is null-terminated
		size_t destsize; // if 0, dest is not written
		uint32_t defaultchar; // if 0, stops immediately after a error
		size_t sourceresult; // out
		size_t destresult; // out
	R_END()
	};

	R_ENUM( name = translateresult )
	namespace translate
	{
		enum
		{
			success = 1,
			source_overrun = 2,
			source_corrupted = 3,
			dest_unsupported = 4,
			dest_overrun = 5,
		};
	}

	int translatestr( translation_t* translation );

	R_CLASS( name = encoding )
	struct Encoding
	{
		R_METHOD() static encoding_t const* getencoding( int index );
		R_METHOD() static int translatestr( translation_t* translation );
	};
}

#endif
