#include <renderer-d3d9/program.hpp>
#include <renderer-d3d9/programtranslator.hpp>
#include <renderer-d3d9/context.hpp>
#include <renderer-d3d9/common.hpp>
#include <common/string.hpp>
#include <cstring>

namespace graphics
{
	namespace
	{
		Ref< DataBuffer > strtodb( char const* str )
		{
			size_t len = strlen( str );
			return DataBuffer::create( len, len, str );
		}

		bool compileshader(
			DataBuffer* text,
			Ref< DataBuffer >* plog, Ref< ID3DXBuffer >* pbytecode,
			char const* lang )
		{
			try
			{
				ID3DXBuffer* vbytecode = 0;
				ID3DXBuffer* vlog = 0;
				Ref< ID3DXBuffer > bytecode;
				Ref< ID3DXBuffer > log;
				HRESULT hr = D3DXCompileShader(
					( char const* )text->m_data,
					text->m_length,
					0,
					0,
					"main",
					lang,
					0,
					&vbytecode,
					&vlog,
					0 );
				bytecode.possess( vbytecode );
				log.possess( vlog );
				if( log )
				{
					size_t length = log->GetBufferSize();
					void const* data = log->GetBufferPointer();
					*plog = DataBuffer::create( length, length, data );
				}
				checkerror( hr );
				*pbytecode = std::move( bytecode );
				return true;
			}
			catch( std::exception const& e )
			{
				if( !*plog )
				{
					*plog = strtodb( e.what() );
				}
				return false;
			}
		}
	}

	void Program::doadvance()
	{
		if( m_ready.load( std::memory_order_relaxed ) )
		{
			return;
		}
		try
		{
			Ref< ID3DXBuffer > vertexbytecode;
			Ref< ID3DXBuffer > fragmentbytecode;
			void const* vbuffer;
			void const* fbuffer;
			if( m_source )
			{
				std::atomic_thread_fence( std::memory_order_acquire );
				Ref< DataBuffer > vsrc;
				Ref< DataBuffer > fsrc;
				translateprogram( m_source, &vsrc, &fsrc );
				if( !compileshader( vsrc, &m_log, &vertexbytecode, "vs_3_0" ) )
				{
					m_valid = false;
					goto end;
				}
				if( !compileshader( fsrc, &m_log, &fragmentbytecode, "ps_3_0" ) )
				{
					m_valid = false;
					goto end;
				}
				vbuffer = vertexbytecode->GetBufferPointer();
				fbuffer = fragmentbytecode->GetBufferPointer();
			}
			else if( m_binary )
			{
				std::atomic_thread_fence( std::memory_order_acquire );
				if( m_binary->m_length < sizeof( uint32_t ) )
				{
					throw std::runtime_error( "invalid binary" );
				}
				uint32_t* pvsize = ( uint32_t* )m_binary->m_data;
				size_t vsize = *pvsize;
				if( m_binary->m_length < vsize + sizeof( uint32_t ) )
				{
					throw std::runtime_error( "invalid binary" );
				}
				vbuffer = m_binary->m_data + sizeof( uint32_t );
				fbuffer = m_binary->m_data + vsize + sizeof( uint32_t );
			}
			else
			{
				throw std::runtime_error( "shader source or binary required" );
			}
			IDirect3DVertexShader9* vsh = 0;
			checkerror( Context::Device->CreateVertexShader(
				( DWORD const* )vbuffer,
				&vsh ) );
			m_vertexshader.possess( vsh );
			IDirect3DPixelShader9* fsh = 0;
			checkerror( Context::Device->CreatePixelShader(
				( DWORD const* )fbuffer,
				&fsh ) );
			m_fragmentshader.possess( fsh );
			m_valid = true;
			if( m_cache )
			{
				if( !m_binary )
				{
					size_t vsize = vertexbytecode->GetBufferSize();
					size_t fsize = fragmentbytecode->GetBufferSize();
					size_t binsize = vsize + fsize + sizeof( uint32_t );
					m_binary = DataBuffer::create( binsize, binsize, 0 );
					uint32_t* pvsize = ( uint32_t* )m_binary->m_data;
					*pvsize = vsize;
					memcpy(
						m_binary->m_data + sizeof( uint32_t ),
						vertexbytecode->GetBufferPointer(), vsize );
					memcpy(
						m_binary->m_data + vsize + sizeof( uint32_t ),
						fragmentbytecode->GetBufferPointer(), fsize );
				}
			}
			else
			{
				clearcache();
			}
		}
		catch( std::exception const& e )
		{
			m_log = strtodb( e.what() );
			m_valid = false;
		}
	end:
		m_ready.store( true, std::memory_order_release );
	}

	Program::Program( DataBuffer* binary, bool cache )
		: m_cache( cache )
		, m_ready( false )
	{
		std::atomic_thread_fence( std::memory_order_release );
		m_binary = binary;
		if( cache )
		{
			deferadvance();
		}
	}

	Program::Program( DataBuffer* source, bool cache, int )
		: m_cache( cache )
		, m_ready( false )
	{
		std::atomic_thread_fence( std::memory_order_release );
		m_source = source;
		if( cache )
		{
			deferadvance();
		}
	}

	Program::~Program()
	{
	}

	bool Program::bind()
	{
		if( !m_valid || !m_vertexshader || !m_fragmentshader )
		{
			return false;
		}
		checkerror( Context::Device->SetVertexShader( m_vertexshader ) );
		checkerror( Context::Device->SetPixelShader( m_fragmentshader ) );
		return true;
	}

	void Program::clearcache()
	{
		m_source = nullptr;
		m_binary = nullptr;
		m_log = nullptr;
	}
}
