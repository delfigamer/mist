#include "shader.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <cstring>

namespace graphics
{
	void Shader::doadvance( IDirect3DDevice9* device, int framecount )
	{
		lock_t _lock_guard( m_mutex );
		for( int i = 0; i < 8; ++i )
		{
			Texture* tex = m_textures[ i ];
			if( tex ) {
				tex->advance( device, framecount );
			}
		}
		if( m_vertexshaderdata )
		{
			checkerror( device->CreateVertexShader( ( DWORD const* )m_vertexshaderdata->GetBufferPointer(), &m_vertexshader ) );
			RELEASE( m_vertexshaderdata );
		}
		if( m_pixelshaderdata )
		{
			checkerror( device->CreatePixelShader( ( DWORD const* )m_pixelshaderdata->GetBufferPointer(), &m_pixelshader ) );
			RELEASE( m_pixelshaderdata );
		}
	}

	Shader::Shader()
		: m_vertexshader( 0 )
		, m_pixelshader( 0 )
	{
	}

	Shader::~Shader()
	{
		RELEASE( m_vertexshader );
		RELEASE( m_pixelshader );
	}

	void Shader::bind( IDirect3DDevice9* device, utils::Ref< Texture > const* textures )
	{
		lock_t _lock_guard( m_mutex );
		if( !m_vertexshader || !m_pixelshader )
		{
			checkerror( device->SetVertexShader( 0 ) );
			checkerror( device->SetPixelShader( 0 ) );
			return;
		}
		for( int i = 0; i < 8; ++i )
		{
			int index = m_samplerindex[ i ];
			if( index != -1 )
			{
				Texture* tex = textures[ i ];
				if( !tex )
				{
					tex = m_textures[ i ];
				}
				if( tex )
				{
					tex->bind( device, index );
				}
				else
				{
					checkerror( device->SetTexture( index, 0 ) );
				}
			}
		}
		checkerror( device->SetVertexShader( m_vertexshader ) );
		checkerror( device->SetPixelShader( m_pixelshader ) );
	}

	void Shader::settexture( int stage, Texture* texture ) {
		TABLE_ASSERT( m_textures, stage, "stage" ) = texture;
	}

	void Shader::setshadersources( char const* vert, char const* frag, char const* texnames )
	{
		lock_t _lock_guard( m_mutex );
		ID3DXBuffer* errormsgs = 0;
		HRESULT hr = D3DXCompileShader(
			vert,
			strlen( vert ),
			0,
			0,
			"main",
			"vs_3_0",
			0,
			&m_vertexshaderdata,
			&errormsgs,
			&m_vertexshaderconstants );
		if( errormsgs )
		{
			LOG( "%s", ( char const* )errormsgs->GetBufferPointer() );
			RELEASE( errormsgs );
		}
		checkerror( hr );
		hr = D3DXCompileShader(
			frag,
			strlen( frag ),
			0,
			0,
			"main",
			"ps_3_0",
			0,
			&m_pixelshaderdata,
			&errormsgs,
			&m_pixelshaderconstants );
		if( errormsgs )
		{
			LOG( "%s", ( char const* )errormsgs->GetBufferPointer() );
			RELEASE( errormsgs );
		}
		checkerror( hr );
		// static char const* RegSet[] =
		// {
			// "D3DXRS_BOOL",
			// "D3DXRS_INT4",
			// "D3DXRS_FLOAT4",
			// "D3DXRS_SAMPLER",
		// };
		// static char const* CClass[] =
		// {
			// "D3DXPC_SCALAR",
			// "D3DXPC_VECTOR",
			// "D3DXPC_MATRIX_ROWS",
			// "D3DXPC_MATRIX_COLUMNS",
			// "D3DXPC_OBJECT",
			// "D3DXPC_STRUCT",
		// };
		// static char const* CType[] =
		// {
			// "D3DXPT_VOID",
			// "D3DXPT_BOOL",
			// "D3DXPT_INT",
			// "D3DXPT_FLOAT",
			// "D3DXPT_STRING",
			// "D3DXPT_TEXTURE",
			// "D3DXPT_TEXTURE1D",
			// "D3DXPT_TEXTURE2D",
			// "D3DXPT_TEXTURE3D",
			// "D3DXPT_TEXTURECUBE",
			// "D3DXPT_SAMPLER",
			// "D3DXPT_SAMPLER1D",
			// "D3DXPT_SAMPLER2D",
			// "D3DXPT_SAMPLER3D",
			// "D3DXPT_SAMPLERCUBE",
			// "D3DXPT_PIXELSHADER",
			// "D3DXPT_VERTEXSHADER",
			// "D3DXPT_PIXELFRAGMENT",
			// "D3DXPT_VERTEXFRAGMENT",
			// "D3DXPT_UNSUPPORTED",
		// };
		// for( int i = 0; true; ++i )
		// {
			// D3DXHANDLE handle = m_vertexshaderconstants->GetConstant( 0, i );
			// if( handle )
			// {
				// LOG( "vs %i %#18" PRIxPTR, i, uintptr_t( handle ) );
				// D3DXCONSTANT_DESC desc[ 8 ];
				// int len = 8;
				// checkerror( m_vertexshaderconstants->GetConstantDesc( handle, desc, ( UINT* )&len ) );
				// for( int j = 0; j < len; ++j )
				// {
					// LOG( "desc %i", j );
					// LOG( "Name - %s", desc[ j ].Name );
					// LOG( "RegisterSet - %s", RegSet[ desc[ j ].RegisterSet ] );
					// LOG( "RegisterIndex - %i", desc[ j ].RegisterIndex );
					// LOG( "RegisterCount - %i", desc[ j ].RegisterCount );
					// LOG( "Class - %s", CClass[ desc[ j ].Class ] );
					// LOG( "Type - %s", CType[ desc[ j ].Type ] );
					// LOG( "Rows - %i", desc[ j ].Rows );
					// LOG( "Columns - %i", desc[ j ].Columns );
					// LOG( "Elements - %i", desc[ j ].Elements );
					// LOG( "StructMembers - %i", desc[ j ].StructMembers );
					// LOG( "Bytes - %i", desc[ j ].Bytes );
				// }
			// }
			// else
			// {
				// break;
			// }
		// }
		// for( int i = 0; true; ++i )
		// {
			// D3DXHANDLE handle = m_pixelshaderconstants->GetConstant( 0, i );
			// if( handle )
			// {
				// LOG( "ps %i %#18" PRIxPTR, i, uintptr_t( handle ) );
				// D3DXCONSTANT_DESC desc[ 8 ];
				// int len = 8;
				// checkerror( m_pixelshaderconstants->GetConstantDesc( handle, desc, ( UINT* )&len ) );
				// for( int j = 0; j < len; ++j )
				// {
					// LOG( "desc %i", j );
					// LOG( "Name - %s", desc[ j ].Name );
					// LOG( "RegisterSet - %s", RegSet[ desc[ j ].RegisterSet ] );
					// LOG( "RegisterIndex - %i", desc[ j ].RegisterIndex );
					// LOG( "RegisterCount - %i", desc[ j ].RegisterCount );
					// LOG( "Class - %s", CClass[ desc[ j ].Class ] );
					// LOG( "Type - %s", CType[ desc[ j ].Type ] );
					// LOG( "Rows - %i", desc[ j ].Rows );
					// LOG( "Columns - %i", desc[ j ].Columns );
					// LOG( "Elements - %i", desc[ j ].Elements );
					// LOG( "StructMembers - %i", desc[ j ].StructMembers );
					// LOG( "Bytes - %i", desc[ j ].Bytes );
					// if( desc[ j ].Type == D3DXPT_SAMPLER2D )
					// {
						// LOG( "Sampler index - %i", m_pixelshaderconstants->GetSamplerIndex( handle ) );
					// }
				// }
			// }
			// else
			// {
				// break;
			// }
		// }
		for( int index = 0; index < 8; ++index )
		{
			int scpos = strcspn( texnames, ";" );
			if( scpos > 0 )
			{
				if( scpos < 256 )
				{
					char name[ 256 ];
					memcpy( name, texnames, scpos );
					name[ scpos ] = 0;
					m_samplerindex[ index ] = m_pixelshaderconstants->GetSamplerIndex( name );
					LOG( "%i %s -> %i", index, name, m_samplerindex[ index ] );
				}
				else
				{
					char* name = new char[ scpos + 1 ];
					memcpy( name, texnames, scpos );
					name[ scpos ] = 0;
					m_samplerindex[ index ] = m_pixelshaderconstants->GetSamplerIndex( name );
					LOG( "%i %s -> %i", index, name, m_samplerindex[ index ] );
					delete[] name;
				}
			}
			else
			{
				m_samplerindex[ index ] = -1;
				LOG( "%i -> -1", index );
			}
			if( texnames[ scpos ] )
			{
				texnames = texnames + scpos + 1;
			}
			else
			{
				texnames = texnames + scpos;
			}
		}
	}

	extern "C" {
		Shader* graphics_shader_new() noexcept
		{
		CBASE_PROTECT(
			return new Shader();
		)
		}

		bool graphics_shader_settexture( Shader* sh, int stage, Texture* texture ) noexcept
		{
		CBASE_PROTECT(
			sh->settexture( stage, texture );
			return 1;
		)
		}

		bool graphics_shader_setshadersources( Shader* sh, char const* vert, char const* frag, char const* texnames ) noexcept
		{
		CBASE_PROTECT(
			sh->setshadersources( vert, frag, texnames );
			return 1;
		)
		}
	}
}
