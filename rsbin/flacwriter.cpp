#include <rsbin/flacwriter.hpp>
#include <rsbin/flaccommon.hpp>
#include <common/strexception.hpp>
#include <stdexcept>

namespace rsbin
{
	// Somehow, these symbols fail to import correctly from libFLAC
	namespace
	{
		char const* const FLAC__StreamEncoderStateString[] = {
			"FLAC__STREAM_ENCODER_OK",
			"FLAC__STREAM_ENCODER_UNINITIALIZED",
			"FLAC__STREAM_ENCODER_OGG_ERROR",
			"FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR",
			"FLAC__STREAM_ENCODER_VERIFY_MISMATCH_IN_AUDIO_DATA",
			"FLAC__STREAM_ENCODER_CLIENT_ERROR",
			"FLAC__STREAM_ENCODER_IO_ERROR",
			"FLAC__STREAM_ENCODER_FRAMING_ERROR",
			"FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR",
		};

		char const* const FLAC__StreamEncoderInitStatusString[] = {
			"FLAC__STREAM_ENCODER_INIT_STATUS_OK",
			"FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR",
			"FLAC__STREAM_ENCODER_INIT_STATUS_UNSUPPORTED_CONTAINER",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_CALLBACKS",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_NUMBER_OF_CHANNELS",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BITS_PER_SAMPLE",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_SAMPLE_RATE",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BLOCK_SIZE",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_MAX_LPC_ORDER",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_QLP_COEFF_PRECISION",
			"FLAC__STREAM_ENCODER_INIT_STATUS_BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER",
			"FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE",
			"FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA",
			"FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED",
		};
	}

	FLAC__StreamEncoderWriteStatus FlacWriter::write_callback(
		const FLAC__StreamEncoder* encoder,
		const FLAC__byte* buffer, size_t bytes,
		unsigned samples, unsigned current_frame, void* client_data )
	{
		FlacWriter* writer = ( FlacWriter* )client_data;
		try
		{
			uint64_t result = writer->m_target->blockwrite(
				writer->m_targetpos, bytes, buffer );
			writer->m_targetpos += result;
			return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		}
		catch( std::exception const& e )
		{
			writer->m_error = e.what();
			return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
		}
		catch( ... )
		{
			writer->m_error = "Unknown error";
			return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
		}
	}

	FLAC__StreamEncoderSeekStatus FlacWriter::seek_callback(
		const FLAC__StreamEncoder* encoder,
		FLAC__uint64 absolute_byte_offset, void* client_data )
	{
		FlacWriter* writer = ( FlacWriter* )client_data;
		writer->m_targetpos = absolute_byte_offset;
		return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
	}

	FLAC__StreamEncoderTellStatus FlacWriter::tell_callback(
		const FLAC__StreamEncoder* encoder,
		FLAC__uint64* absolute_byte_offset, void* client_data )
	{
		FlacWriter* writer = ( FlacWriter* )client_data;
		*absolute_byte_offset = writer->m_targetpos;
		return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
	}

	FlacWriter::FlacWriter(
		int bitdepth, int channels, int samplerate,
		DataBuffer* data )
		: m_bitdepth( bitdepth )
		, m_channels( channels )
		, m_samplerate( samplerate )
		, m_data( data )
		, m_encoder( 0 )
		, m_targetpos( 0 )
	{
		if( !Flac::isvalidformat( m_bitdepth, m_channels, m_samplerate ) )
		{
			throw std::runtime_error( "invalid sound format" );
		}
		m_encoder = FLAC__stream_encoder_new();
		if( !m_encoder )
		{
			throw std::runtime_error( "failed to create the flac encoder" );
		}
		m_target.emplace();
		FLAC__stream_encoder_set_channels(
			m_encoder, m_channels );
		FLAC__stream_encoder_set_bits_per_sample(
			m_encoder, m_bitdepth );
		FLAC__stream_encoder_set_sample_rate(
			m_encoder, m_samplerate );
		FLAC__stream_encoder_set_total_samples_estimate(
			m_encoder,
			m_data->m_length / ( 4 * m_channels ) );
		FLAC__StreamEncoderInitStatus status = FLAC__stream_encoder_init_stream(
			m_encoder,
			&FlacWriter::write_callback,
			&FlacWriter::seek_callback,
			&FlacWriter::tell_callback,
			0,
			this );
		if( status == FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR )
		{
			FLAC__StreamEncoderState state =
				FLAC__stream_encoder_get_state( m_encoder );
			FLAC__stream_encoder_delete( m_encoder );
			if( m_error )
			{
				throw StrException( m_error );
			}
			else
			{
				throw std::runtime_error(
					FLAC__StreamEncoderStateString[ state ] );
			}
		}
		else if( status != FLAC__STREAM_ENCODER_INIT_STATUS_OK )
		{
			FLAC__stream_encoder_delete( m_encoder );
			if( m_error )
			{
				throw StrException( m_error );
			}
			else
			{
				throw std::runtime_error(
					FLAC__StreamEncoderInitStatusString[ status ] );
			}
		}
		bool success = FLAC__stream_encoder_process_interleaved(
			m_encoder,
			( int32_t* )m_data->m_data, unsigned( m_data->m_length / ( 4 * m_channels ) ) );
		if( success )
		{
			success = FLAC__stream_encoder_finish( m_encoder );
		}
		if( !success )
		{
			FLAC__StreamEncoderState state =
				FLAC__stream_encoder_get_state( m_encoder );
			FLAC__stream_encoder_delete( m_encoder );
			if( m_error )
			{
				throw StrException( m_error );
			}
			else
			{
				throw std::runtime_error(
					FLAC__StreamEncoderStateString[ state ] );
			}
		}
	}

	FlacWriter::~FlacWriter()
	{
		if( m_encoder )
		{
			FLAC__stream_encoder_delete( m_encoder );
		}
	}
}
