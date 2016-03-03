#include "shader.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <atomic>
#include <cstring>

namespace graphics
{
	static char const* const shlangtable[] = {
		"ps_3_0",
		"vs_3_0",
	};

	void Shader::doadvance()
	{
		if( !m_shader )
		{
			try
			{
				utils::DataBuffer* source = m_source;
				std::atomic_thread_fence( std::memory_order_acquire );
				ID3DXBuffer* log = 0;
				ID3DXBuffer* bytecode = 0;
				HRESULT hr = D3DXCompileShader(
					( char const* )source->m_data,
					source->m_length,
					0,
					0,
					"main",
					shlangtable[ m_stage ],
					0,
					&bytecode,
					&log,
					&m_constants );
				if( log )
				{
					m_log.setchars( ( char const* )log->GetBufferPointer() );
					RELEASE( log );
				}
				checkerror( hr );
				switch( m_stage )
				{
				case ShaderStage_Vertex:
					checkerror( Context::Device->CreateVertexShader(
						( DWORD const* )bytecode->GetBufferPointer(),
						&m_vertexshader ) );
					break;

				case ShaderStage_Fragment:
					checkerror( Context::Device->CreatePixelShader(
						( DWORD const* )bytecode->GetBufferPointer(),
						&m_pixelshader ) );
					break;
				}
				RELEASE( bytecode );
				std::atomic_thread_fence( std::memory_order_release );
				m_source = nullptr;
			}
			catch( std::exception const& e )
			{
				if( !m_log )
				{
					m_log.setchars( e.what() );
				}
				std::atomic_thread_fence( std::memory_order_release );
				m_source = nullptr;
			}
		}
	}

	Shader::Shader( int format, int stage, utils::DataBuffer* source )
		: m_stage( 0 )
		, m_shader( 0 )
		, m_constants( 0 )
	{
		if( format != ShaderFormat_HLSL_3_0_Source )
		{
			throw std::runtime_error( "unsupported shader format" );
		}
		std::atomic_thread_fence( std::memory_order_release );
		m_source = source;
	}

	Shader::~Shader()
	{
		RELEASE( m_constants );
		RELEASE( m_shader );
	}

	void Shader::getstate( bool* ready, bool* valid, char const** log )
	{
		utils::DataBuffer* source = m_source;
		if( !source )
		{
			std::atomic_thread_fence( std::memory_order_acquire );
			*valid = m_shader != 0;
			*log = m_log.getchars();
			*ready = true;
		}
		else
		{
			*ready = false;
		}
	}

	int Shader::getstage()
	{
		return m_stage;
	}

	bool Shader::bind( ID3DXConstantTable** ct )
	{
		if( !m_shader )
		{
			return false;
		}
		switch( m_stage )
		{
		case ShaderStage_Vertex:
			checkerror( Context::Device->SetVertexShader( m_vertexshader ) );
			break;

		case ShaderStage_Fragment:
			checkerror( Context::Device->SetPixelShader( m_pixelshader ) );
			break;
		}
		*ct = m_constants;
		return true;
	}

	bool Shader::isformatsupported( int format ) noexcept
	{
		return format == Shader::ShaderFormat_HLSL_3_0_Source;
	}
}
