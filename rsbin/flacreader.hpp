#pragma once

#include <utils/string.hpp>
#include <utils/cyclicbuffer.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <utils/databuffer.hpp>
#include <common.hpp>
#include <flac/stream_decoder.hpp>
#include <cinttypes>

namespace rsbin
{
	R_CLASS( name = flacreader )
	class FlacReader: public utils::RefObject
	{
	private:
		int m_bitdepth;
		int m_channels;
		int m_samplerate;
		FLAC__StreamDecoder* m_decoder;
		utils::Ref< utils::DataBuffer > m_source;
		utils::CyclicBuffer m_target;
		utils::String m_error;

		static void error_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__StreamDecoderErrorStatus status, void* client_data );
		static FLAC__StreamDecoderReadStatus read_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__byte* buffer, size_t* bytes, void* client_data );
		static FLAC__StreamDecoderWriteStatus write_callback(
			const FLAC__StreamDecoder* decoder,
			const FLAC__Frame* frame,
			const FLAC__int32* const* buffer, void* client_data );

	public:
		FlacReader( utils::DataBuffer* source );
		~FlacReader();
		FlacReader( FlacReader const& ) = delete;
		FlacReader& operator=( FlacReader const& ) = delete;

		R_METHOD() static FlacReader* create( utils::DataBuffer* source )
		{
			return new FlacReader();
		}
		R_METHOD() void feed( size_t length, void const* buffer );
		R_METHOD() size_t grab( size_t length, void* buffer );
		R_METHOD() bool finish();
		R_METHOD() int getbitdepth() NOEXCEPT { return m_bitdepth; }
		R_METHOD() int getchannels() NOEXCEPT { return m_channels; }
		R_METHOD() int getsamplerate() NOEXCEPT { return m_samplerate; }
	};
}
