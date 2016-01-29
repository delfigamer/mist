#ifndef UTILS_ENCODING_HPP__
#define UTILS_ENCODING_HPP__ 1

#include <cinttypes>

namespace utils
{
	// on success - return true, *pointlength <- number of bytes written
	// invalid stream - return false, *pointlength <- 0
	// buffer overrun - return false, *pointlength <- number of bytes that would be written
	// dest may be 0
	// if destsize == 0 && dest == 0, always succeeds
	// if destsize == 0 && dest != 0, always fails
	typedef bool( *encoder_t )(
		void* dest,
		uint32_t charcode,
		size_t destsize,
		size_t* pointlength );

	// on success - return true, *pointlength <- number of bytes consumed
	// invalid stream - return false, *pointlength <- number of bytes consumed
	// buffer overrun - return false, *pointlength <- 0
	// charcode may be 0
	// if sourcesize == 0, source is assumed to be unbound
	typedef bool( *decoder_t )(
		void const* source,
		uint32_t* charcode,
		size_t sourcesize,
		size_t* pointlength );

	struct encoding_t
	{
		encoder_t encode;
		decoder_t decode;
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
		enum indices
		{
			index_utf8 = 0,
			index_utf16 = 1,
			index_utf32 = 2,
			index_utf16le = 3,
			index_utf16be = 4,
			index_utf32le = 5,
			index_utf32be = 6,
		};
	}

	struct translation_t
	{
		encoding_t const* senc;
		encoding_t const* denc;
		void const* source;
		void* dest; // may be 0
		size_t sourcesize; // if 0, source is null-terminated
		size_t destsize; // if 0, dest is not written
		uint32_t defaultchar; // if 0, stops immediately after a error
		size_t sourceresult; // out
		size_t destresult; // out
	};

	int translatestr( translation_t* translation );

	enum
	{
		translate_success = 1,
		translate_source_overrun = 2,
		translate_source_corrupted = 3,
		translate_dest_unsupported = 4,
		translate_dest_overrun = 5,
	};

	encoding_t const* utils_encoding_getencoding( int index ) noexcept;
	int utils_encoding_translatestr( translation_t* translation ) noexcept;
}

#endif
