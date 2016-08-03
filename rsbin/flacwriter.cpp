#include <rsbin/flacwriter.hpp>
#include <rsbin/flaccommon.hpp>
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <stdexcept>

namespace rsbin
{
	enum
	{
		BlockSize = 0x10000,
	};

	FLAC__StreamEncoderWriteStatus FlacWriter::write_callback(
		const FLAC__StreamEncoder* encoder,
		const FLAC__byte* buffer, size_t bytes,
		unsigned samples, unsigned current_frame, void* client_data )
	{
		FlacWriter* writer = ( FlacWriter* )client_data;
		try
		{
			writer->m_buffer.push( bytes, buffer );
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

	bool FlacWriter::feedbuffer()
	{
		size_t rest = m_data->m_length / ( 4 * m_channels ) - m_position;
		if( rest > 0 )
		{
			if( rest > BlockSize )
			{
				rest = BlockSize;
			}
			int32_t* data = ( int32_t* )m_data->m_data;
			if( !FLAC__stream_encoder_process_interleaved(
				m_encoder,
				data + m_position * m_channels, rest ) )
			{
				FLAC__StreamEncoderState state =
					FLAC__stream_encoder_get_state( m_encoder );
				if( state == FLAC__STREAM_ENCODER_CLIENT_ERROR )
				{
					throw utils::StrException( "%s", m_error.getchars() );
				}
				else
				{
					throw std::runtime_error(
						FLAC__StreamEncoderStateString[ state ] );
				}
			}
			m_position += rest;
			return true;
		}
		else
		{
			if( !m_finished )
			{
				FLAC__stream_encoder_finish( m_encoder );
				m_finished = true;
			}
			return false;
		}
	}

	FlacWriter::FlacWriter(
		int bitdepth, int channels, int samplerate,
		utils::DataBuffer* data )
		: m_bitdepth( bitdepth )
		, m_channels( channels )
		, m_samplerate( samplerate )
		, m_position( 0 )
		, m_finished( false )
		, m_data( data )
		, m_encoder( 0 )
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
			0,
			0,
			0,
			this );
		if( status == FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR )
		{
			FLAC__StreamEncoderState state =
				FLAC__stream_encoder_get_state( m_encoder );
			FLAC__stream_encoder_delete( m_encoder );
			throw std::runtime_error(
				FLAC__StreamEncoderStateString[ state ] );
		}
		else if( status != FLAC__STREAM_ENCODER_INIT_STATUS_OK )
		{
			FLAC__stream_encoder_delete( m_encoder );
			throw std::runtime_error(
				FLAC__StreamEncoderInitStatusString[ status ] );
		}
	}

	FlacWriter::~FlacWriter()
	{
		if( m_encoder )
		{
			FLAC__stream_encoder_delete( m_encoder );
		}
	}

	size_t FlacWriter::grab( size_t length, void* buffer )
	{
		size_t result = 0;
		while( length > 0 )
		{
			size_t advance = m_buffer.pop( length, buffer );
			length -= advance;
			buffer = ( uint8_t* )buffer + advance;
			result += advance;
			if( !feedbuffer() )
			{
				break;
			}
		}
		size_t advance = m_buffer.pop( length, buffer );
		length -= advance;
		buffer = ( uint8_t* )buffer + advance;
		result += advance;
		return result;
	}

	bool FlacWriter::isfinished()
	{
		return m_position == m_data->m_length / ( 4 * m_channels )
			&& m_buffer.isempty();
	}
}
