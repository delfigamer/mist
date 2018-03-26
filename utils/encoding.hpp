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

	struct [[ r::class ]] encoding_t
	{
		encoder_t encoder;
		decoder_t decoder;

		[[ r::method ]] bool encode(
			void* dest,
			uint32_t charcode,
			size_t destsize,
			size_t* pointlength [[ r::required ]] ) const noexcept
		{
			return encoder( dest, charcode, destsize, pointlength );
		}

		[[ r::method ]] bool decode(
			void const* source [[ r::required ]],
			uint32_t* charcode [[ r::required ]],
			size_t sourcesize,
			size_t* pointlength [[ r::required ]] ) const noexcept
		{
			return decoder( source, charcode, sourcesize, pointlength );
		}
	};

	namespace encoding
	{
		[[ r::object ]] extern encoding_t const utf8;
		[[ r::object ]] extern encoding_t const utf16;
		[[ r::object ]] extern encoding_t const utf32;
		[[ r::object ]] extern encoding_t const utf16le;
		[[ r::object ]] extern encoding_t const utf16be;
		[[ r::object ]] extern encoding_t const utf32le;
		[[ r::object ]] extern encoding_t const utf32be;
	}

	struct [[ r::struct ]] translation_t
	{
		void const* source;
		void* dest; // may be nullptr
		size_t sourcesize; // if 0, source is null-terminated
		size_t destsize; // if 0, dest is not written
		uint32_t defaultchar; // if 0, stops immediately after an error
		size_t sourceresult; // out, the number of bytes read
		size_t destresult; // out, the number of bytes that were or would've been written
	};

	enum class [[ r::enum ]] translateresult
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

	[[ r::method ]]
	translateresult translatestr(
		encoding_t const* senc [[ r::required ]],
		encoding_t const* denc [[ r::required ]],
		translation_t* translation [[ r::required ]] );
	Ref< DataBuffer > translatebuffer(
		encoding_t const* senc, encoding_t const* denc,
		void const* source, size_t sourcesize = 0 );
	Ref< DataBuffer > translatebuffer(
		encoding_t const* senc, encoding_t const* denc,
		DataBuffer* source );
}
