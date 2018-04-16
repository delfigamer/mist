#pragma once

#include <common/ref.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>

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
	}

	struct translation_t
	{
		encoding_t const* senc;
		encoding_t const* denc;
		void const* source;
		void* dest; // may be nullptr
		size_t sourcesize; // if 0, source is null-terminated
		size_t destsize; // if 0, dest is not written
		uint32_t defaultchar; // if 0, stops immediately after an error
		size_t sourceresult; // out, the number of bytes read
		size_t destresult; // out, the number of bytes that were or would've been written
	};

	enum class translateresult
	{
		success,
		// sourcesize limit was hit in the middle of a codepoint
		source_overrun,
		// an invalid codepoint was encountered and defaultchar == 0
		source_corrupted,
		// destination encoding cannot represent a given character
		dest_unsupported,
		// destsize was hit
		dest_overrun,
	};

	translateresult translatestr(
		translation_t& translation );
	Ref< DataBuffer > translatebuffer(
		encoding_t const* senc, encoding_t const* denc,
		void const* source, size_t sourcesize = 0 );
	Ref< DataBuffer > translatebuffer(
		encoding_t const* senc, encoding_t const* denc,
		DataBuffer* source );
}

namespace utf8
{
	[[ r::method ]] inline bool encode(
		void* dest,
		uint32_t charcode,
		size_t destsize,
		size_t* pointlength ) noexcept
	{
		return utils::encoding::utf8.encode(
			dest, charcode, destsize, pointlength );
	}

	[[ r::method ]] inline bool decode(
		void const* source,
		uint32_t* charcode,
		size_t sourcesize,
		size_t* pointlength ) noexcept
	{
		return utils::encoding::utf8.decode(
			source, charcode, sourcesize, pointlength );
	}
}
