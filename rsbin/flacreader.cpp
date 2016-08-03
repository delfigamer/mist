#include <rsbin/flacreader.hpp>
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <cstring>

namespace rsbin
{
	enum
	{
		BlockSize = 0x10000,
	};

	void FlacReader::error_callback(
			const FLAC__StreamDecoder* decoder,
			FLAC__StreamDecoderErrorStatus status, void* client_data )
	{
		FlacReader* reader = ( FlacReader* )client_data;
		reader->m_error.setchars( FLAC__StreamDecoderStateString[ status ] );
	}

	FLAC__StreamDecoderReadStatus read_callback(
		const FLAC__StreamDecoder* decoder,
		FLAC__byte* buffer, size_t* bytes, void* client_data )
	{
	}

	FLAC__StreamDecoderWriteStatus write_callback(
		const FLAC__StreamDecoder* decoder,
		const FLAC__Frame* frame,
		const FLAC__int32* const* buffer, void* client_data )
	{
	}

	FlacReader::FlacReader()
		: m_bitdepth( 0 )
		, m_channels( 0 )
		, m_samplerate( 0 )
		, m_decoder( 0 )
		, m_lstate( 0 )
		, m_ltread( 0 )
	{
		m_decoder = FLAC__stream_decoder_new();
		FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(
			m_decoder,
			&FlacReader::read_callback,
			0,
			0,
			0,
			0,
			&FlacReader::write_callback,
			0,
			&FlacReader::error_callback,
			this );
		m_lstate = luaL_newstate();
		m_lthread = lua_newthread( m_lstate );
		lua_pushcfunction( m_lthread, &FlacReader::reader_thread );
	}

	FlacReader::~FlacReader()
	{
		if( m_decoder )
		{
			FLAC__stream_decoder_delete( m_decoder );
		}
	}

	void FlacReader::feed( size_t length, void const* buffer )
	{
	}

	size_t FlacReader::grab( size_t length, void* buffer )
	{
	}

	bool FlacReader::finish()
	{
	}
}
