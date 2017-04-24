#pragma once

#include <rsbin/memoryio.hpp>
#include <common/string.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <flac/stream_decoder.h>

namespace rsbin
{
	R_CLASS( name = flacreader )
	class FlacReader: public RefObject
	{
	private:
		int m_bitdepth;
		int m_channels;
		int m_samplerate;
		FLAC__StreamDecoder* m_decoder;
		Ref< MemoryIo > m_source;
		uint64_t m_sourcepos;
		Ref< DataBuffer > m_target;
		size_t m_totalsamples;
		String m_error;

		static void error_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__StreamDecoderErrorStatus status, void* client_data );
		static FLAC__StreamDecoderReadStatus read_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__byte* buffer, size_t* bytes, void* client_data );
		static FLAC__StreamDecoderSeekStatus seek_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__uint64 absolute_byte_offset, void* client_data );
		static FLAC__StreamDecoderTellStatus tell_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__uint64* absolute_byte_offset, void* client_data );
		static FLAC__StreamDecoderLengthStatus length_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__uint64* stream_length, void* client_data );
		static FLAC__bool eof_callback(
			const FLAC__StreamDecoder* decoder, void* client_data );
		static FLAC__StreamDecoderWriteStatus write_callback(
			const FLAC__StreamDecoder* decoder,
			const FLAC__Frame* frame,
			const FLAC__int32* const* buffer, void* client_data );

	public:
		FlacReader( MemoryIo* source );
		~FlacReader();
		FlacReader( FlacReader const& ) = delete;
		FlacReader& operator=( FlacReader const& ) = delete;

		R_METHOD() static FlacReader* create( MemoryIo* source )
		{
			return new FlacReader( source );
		}
		R_METHOD() int getbitdepth() NOEXCEPT { return m_bitdepth; }
		R_METHOD() int getchannels() NOEXCEPT { return m_channels; }
		R_METHOD() int getsamplerate() NOEXCEPT { return m_samplerate; }
		R_METHOD() DataBuffer* getdata() NOEXCEPT
		{
			::addref( m_target );
			return m_target;
		}
		R_METHOD() bool isfinished() NOEXCEPT { return true; }
	};
}
