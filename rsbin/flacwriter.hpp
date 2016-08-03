#pragma once

#include <utils/string.hpp>
#include <utils/cyclicbuffer.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <utils/databuffer.hpp>
#include <common.hpp>
#include <flac/stream_encoder.h>
#include <cinttypes>

namespace rsbin
{
	R_CLASS( name = flacwriter )
	class FlacWriter: public utils::RefObject
	{
	private:
		int m_bitdepth;
		int m_channels;
		int m_samplerate;
		size_t m_position;
		bool m_finished;
		utils::Ref< utils::DataBuffer > m_data;
		FLAC__StreamEncoder* m_encoder;
		utils::CyclicBuffer m_buffer;
		utils::String m_error;

		static FLAC__StreamEncoderWriteStatus write_callback(
			const FLAC__StreamEncoder* encoder,
			const FLAC__byte* buffer, size_t bytes,
			unsigned samples, unsigned current_frame, void* client_data );
		bool feedbuffer();

	public:
		FlacWriter(
			int bitdepth, int channels, int samplerate,
			utils::DataBuffer* data );
		~FlacWriter();
		FlacWriter( FlacWriter const& ) = delete;
		FlacWriter& operator=( FlacWriter const& ) = delete;

		R_METHOD() static FlacWriter* create(
			int bitdepth, int channels, int samplerate,
			utils::DataBuffer* data )
		{
			return new FlacWriter( bitdepth, channels, samplerate, data );
		}
		R_METHOD() size_t grab( size_t length, void* buffer );
		R_METHOD() bool isfinished();
	};
}
