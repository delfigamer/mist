#include <rsbin/flacreader.hpp>
#include <common/strexception.hpp>
#include <stdexcept>
#include <cstring>

namespace rsbin
{
	// Somehow, these symbols fail to import correctly from libFLAC
	namespace
	{
		char const* const FLAC__StreamDecoderStateString[] = {
			"FLAC__STREAM_DECODER_SEARCH_FOR_METADATA",
			"FLAC__STREAM_DECODER_READ_METADATA",
			"FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC",
			"FLAC__STREAM_DECODER_READ_FRAME",
			"FLAC__STREAM_DECODER_END_OF_STREAM",
			"FLAC__STREAM_DECODER_OGG_ERROR",
			"FLAC__STREAM_DECODER_SEEK_ERROR",
			"FLAC__STREAM_DECODER_ABORTED",
			"FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR",
			"FLAC__STREAM_DECODER_UNINITIALIZED",
		};

		char const* const FLAC__StreamDecoderInitStatusString[] = {
			"FLAC__STREAM_DECODER_INIT_STATUS_OK",
			"FLAC__STREAM_DECODER_INIT_STATUS_UNSUPPORTED_CONTAINER",
			"FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS",
			"FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR",
			"FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE",
			"FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED",
		};
	}

	void FlacReader::error_callback(
		const FLAC__StreamDecoder* decoder,
		FLAC__StreamDecoderErrorStatus status, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		reader->m_error = FLAC__StreamDecoderStateString[ status ];
	}

	FLAC__StreamDecoderReadStatus FlacReader::read_callback(
		const FLAC__StreamDecoder* decoder,
		FLAC__byte* buffer, size_t* bytes, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		*bytes = reader->m_source->blockread(
			reader->m_sourcepos, *bytes, buffer );
		reader->m_sourcepos += *bytes;
		if( *bytes == 0 )
		{
			return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
		}
		else
		{
			return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		}
	}

	FLAC__StreamDecoderSeekStatus FlacReader::seek_callback(
		const FLAC__StreamDecoder* decoder,
		FLAC__uint64 absolute_byte_offset, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		reader->m_sourcepos = absolute_byte_offset;
		return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
	}

	FLAC__StreamDecoderTellStatus FlacReader::tell_callback(
		const FLAC__StreamDecoder* decoder,
		FLAC__uint64* absolute_byte_offset, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		*absolute_byte_offset = reader->m_sourcepos;
		return FLAC__STREAM_DECODER_TELL_STATUS_OK;
	}

	FLAC__StreamDecoderLengthStatus FlacReader::length_callback(
		const FLAC__StreamDecoder* decoder,
		FLAC__uint64* stream_length, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		*stream_length = reader->m_source->getsize();
		return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
	}

	FLAC__bool FlacReader::eof_callback(
		const FLAC__StreamDecoder* decoder, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		uint64_t sourcesize = reader->m_source->getsize();
		return reader->m_sourcepos == sourcesize;
	}

	FLAC__StreamDecoderWriteStatus FlacReader::write_callback(
		const FLAC__StreamDecoder* decoder,
		const FLAC__Frame* frame,
		const FLAC__int32* const* buffer, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		try
		{
			if( reader->m_channels == 0 )
			{
				reader->m_channels = frame->header.channels;
			}
			if( reader->m_bitdepth == 0 )
			{
				reader->m_bitdepth = frame->header.bits_per_sample;
			}
			if( reader->m_samplerate == 0 )
			{
				reader->m_samplerate = frame->header.sample_rate;
			}
			if( reader->m_channels != int( frame->header.channels ) ||
				reader->m_bitdepth != int( frame->header.bits_per_sample ) ||
				reader->m_samplerate != int( frame->header.sample_rate ) )
			{
				reader->m_error = "unsupported FLAC stream";
				return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
			}
			if( !reader->m_target )
			{
				uint64_t samples =
					FLAC__stream_decoder_get_total_samples( decoder );
				if( samples != 0 )
				{
					reader->m_target = DataBuffer::create(
						0, samples * reader->m_channels * 4, nullptr );
				}
				else
				{
					reader->m_target = DataBuffer::create(
						0, frame->header.blocksize * reader->m_channels * 4,
						nullptr );
				}
			}
			if( ( reader->m_totalsamples + frame->header.blocksize ) *
					reader->m_channels * 4 >
				reader->m_target->m_capacity )
			{
				reader->m_target = DataBuffer::create(
					reader->m_target->m_length, reader->m_target->m_capacity * 2,
					reader->m_target->m_data );
			}
			int32_t* targetbuffer = ( int32_t* )reader->m_target->m_data
				+ reader->m_totalsamples * reader->m_channels;
			if( reader->m_channels == 1 )
			{
				memcpy( targetbuffer, buffer[ 0 ], frame->header.blocksize * 4 );
			}
			else if( reader->m_channels == 2 )
			{
				for( size_t i = 0; i < frame->header.blocksize; ++i )
				{
					targetbuffer[ i * 2 ] = buffer[ 0 ][ i ];
					targetbuffer[ i * 2 + 1 ] = buffer[ 1 ][ i ];
				}
			}
			else
			{
				for( size_t i = 0; i < frame->header.blocksize; ++i )
				{
					for( size_t ch = 0; ch < size_t( reader->m_channels ); ++ch )
					{
						targetbuffer[ i * reader->m_channels + ch ] =
							buffer[ ch ][ i ];
					}
				}
			}
			reader->m_totalsamples += frame->header.blocksize;
			reader->m_target->m_length =
				reader->m_totalsamples * reader->m_channels * 4;
			return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
		}
		catch( std::exception const& e )
		{
			reader->m_error = e.what();
			return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT ;
		}
		catch( ... )
		{
			reader->m_error = "Unknown error";
			return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT ;
		}
	}

	FlacReader::FlacReader( MemoryIo* source )
		: m_bitdepth( 0 )
		, m_channels( 0 )
		, m_samplerate( 0 )
		, m_decoder( 0 )
		, m_source( source )
		, m_sourcepos( 0 )
		, m_totalsamples( 0 )
	{
		m_decoder = FLAC__stream_decoder_new();
		if( !m_decoder )
		{
			throw std::runtime_error( "failed to create the flac decoder" );
		}
		FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(
			m_decoder,
			&FlacReader::read_callback,
			&FlacReader::seek_callback,
			&FlacReader::tell_callback,
			&FlacReader::length_callback,
			&FlacReader::eof_callback,
			&FlacReader::write_callback,
			0,
			&FlacReader::error_callback,
			this );
		if( status != FLAC__STREAM_DECODER_INIT_STATUS_OK )
		{
			FLAC__stream_decoder_delete( m_decoder );
			throw std::runtime_error(
				FLAC__StreamDecoderInitStatusString[ status ] );
		}
		if( !FLAC__stream_decoder_process_until_end_of_stream( m_decoder ) )
		{
			FLAC__StreamDecoderState state =
				FLAC__stream_decoder_get_state( m_decoder );
			FLAC__stream_decoder_delete( m_decoder );
			if( !m_error.empty() )
			{
				throw StrException( m_error );
			}
			else
			{
				throw std::runtime_error(
					FLAC__StreamDecoderStateString[ state ] );
			}
		}
	}

	FlacReader::~FlacReader()
	{
		if( m_decoder )
		{
			FLAC__stream_decoder_delete( m_decoder );
		}
	}
}
