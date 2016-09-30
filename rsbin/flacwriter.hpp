#pragma once

#include <rsbin/memoryio.hpp>
#include <common/string.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <flac/stream_encoder.h>
#include <cinttypes>

namespace rsbin
{
	R_CLASS( name = flacwriter )
	class FlacWriter: public RefObject
	{
	private:
		int m_bitdepth;
		int m_channels;
		int m_samplerate;
		Ref< DataBuffer > m_data;
		FLAC__StreamEncoder* m_encoder;
		Ref< MemoryIo > m_target;
		uint64_t m_targetpos;
		String m_error;

		static FLAC__StreamEncoderWriteStatus write_callback(
			const FLAC__StreamEncoder* encoder,
			const FLAC__byte* buffer, size_t bytes,
			unsigned samples, unsigned current_frame, void* client_data );
		static FLAC__StreamEncoderSeekStatus seek_callback(
			const FLAC__StreamEncoder* encoder,
			FLAC__uint64 absolute_byte_offset, void* client_data );
		static FLAC__StreamEncoderTellStatus tell_callback(
			const FLAC__StreamEncoder* encoder,
			FLAC__uint64* absolute_byte_offset, void* client_data );

	public:
		FlacWriter(
			int bitdepth, int channels, int samplerate,
			DataBuffer* data );
		~FlacWriter();
		FlacWriter( FlacWriter const& ) = delete;
		FlacWriter& operator=( FlacWriter const& ) = delete;

		R_METHOD() static FlacWriter* create(
			int bitdepth, int channels, int samplerate,
			DataBuffer* data )
		{
			return new FlacWriter( bitdepth, channels, samplerate, data );
		}
		R_METHOD() bool isfinished() NOEXCEPT { return true; }
		R_METHOD( addref ) MemoryIo* getbuffer() NOEXCEPT { return m_target; }
	};
}
