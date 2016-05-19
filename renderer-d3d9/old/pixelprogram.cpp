#include "pixelprogram.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	static int const optable[] =
	{
		D3DTOP_DISABLE,
		D3DTOP_SELECTARG1,
		D3DTOP_SELECTARG2,
		D3DTOP_MODULATE,
		D3DTOP_MODULATE2X,
		D3DTOP_MODULATE4X,
		D3DTOP_ADD,
		D3DTOP_ADDSIGNED,
		D3DTOP_ADDSIGNED2X,
		D3DTOP_SUBTRACT,
		D3DTOP_ADDSMOOTH,
		D3DTOP_BLENDDIFFUSEALPHA,
		D3DTOP_BLENDTEXTUREALPHA,
		D3DTOP_BLENDFACTORALPHA,
		D3DTOP_DOTPRODUCT3,
	};
	static int const argtable[] =
	{
		D3DTA_CONSTANT,
		D3DTA_CURRENT,
		D3DTA_DIFFUSE,
		D3DTA_TEXTURE,
	};
	static int const texcoordtable[] =
	{
		D3DTSS_TCI_CAMERASPACEPOSITION,
		0,
		1,
	};

	static uint32_t const vertexshaderdata[] =
	{
		0xfffe0300, // vs_3_0
		0x0200001f, 0x80000000, 0x900f0000, // dcl_position v0
		0x0200001f, 0x80000005, 0x900f0001, // dcl_texcoord v1
		0x0200001f, 0x80010005, 0x900f0002, // dcl_texcoord1 v2
		0x0200001f, 0x8000000a, 0x900f0003, // dcl_color v3
		0x0200001f, 0x80000000, 0xe00f0000, // dcl_position o0
		0x0200001f, 0x80000005, 0xe00f0001, // dcl_texcoord o1
		0x0200001f, 0x80010005, 0xe00f0002, // dcl_texcoord1 o2
		0x0200001f, 0x8000000a, 0xe00f0003, // dcl_color o3
		0x03000009, 0xe0010000, 0x90e40000, 0xa0e40000, // dp4 o0.x, v0, c0
		0x03000009, 0xe0020000, 0x90e40000, 0xa0e40001, // dp4 o0.y, v0, c1
		0x03000009, 0xe0040000, 0x90e40000, 0xa0e40002, // dp4 o0.z, v0, c2
		0x03000009, 0xe0080000, 0x90e40000, 0xa0e40003, // dp4 o0.w, v0, c3
		0x02000001, 0xe00f0001, 0x90e40001, // mov o1, v1
		0x02000001, 0xe00f0002, 0x90e40002, // mov o2, v2
		0x02000001, 0xe00f0003, 0x90e40003, // mov o3, v3
		0x0000ffff,
	};

	static uint32_t const pixelshaderdata[] =
	{
		0xffff0300, // ps_3_0
		0x0200001f, 0x80000000, 0x900b0000, // dcl_texcoord v0.xyw
		0x0200001f, 0x90000000, 0xa00f0800, // dcl_2d s0
		0x02000006, 0x80010000, 0x90ff0000, // rcp r0.x, v0.w
		0x03000005, 0x80030000, 0x80000000, 0x90e40000, // mul r0.xy, r0.x, v0
		0x03000042, 0x800f0800, 0x80e40000, 0xa0e40800, // texld oC0, r0, s0
		0x0000ffff,
	};

	void PixelProgram::doadvance( IDirect3DDevice9* device, int framecount )
	{
		lock_t _lock_guard( m_mutex );
		for( int i = 0; i < 8; ++i )
		{
			Texture* tex = m_stages[ i ].texture;
			if( tex ) {
				tex->advance( device, framecount );
			}
		}
		if( !m_vertexshader )
		{
			checkerror( device->CreateVertexShader( ( DWORD const* )vertexshaderdata, &m_vertexshader ) );
		}
		if( !m_pixelshader )
		{
			checkerror( device->CreatePixelShader( ( DWORD const* )pixelshaderdata, &m_pixelshader ) );
		}
	}

#define DEFAULT_STAGE( op ) { \
	nullptr, 0xffffffff, \
	op, D3DTA_TEXTURE, D3DTA_CURRENT, \
	op, D3DTA_TEXTURE, D3DTA_CURRENT, \
	0, 0 \
}
#define DEFAULT_MATRIX() { \
	1, 0, 0, 0, \
	0, 1, 0, 0, \
	0, 0, 1, 0, \
	0, 0, 0, 1 \
}

	PixelProgram::PixelProgram() :
		m_stages{
			DEFAULT_STAGE( D3DTOP_MODULATE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ),
			DEFAULT_STAGE( D3DTOP_DISABLE ) } ,
		m_matrices{ DEFAULT_MATRIX(), DEFAULT_MATRIX() } {
	}

	PixelProgram::~PixelProgram()
	{
		RELEASE( m_vertexshader );
		RELEASE( m_pixelshader );
	}

// 	static D3DMATRIX IdentityMatrix = DEFAULT_MATRIX();

	void PixelProgram::bind( IDirect3DDevice9* device, utils::Ref< Texture > const* textures )
	{
		lock_t _lock_guard( m_mutex );
		// for( int i = 0; i < 8; ++i ) {
			// Stage* stage = &m_stages[ i ];
			// Texture* tex = textures[ i ];
			// if( !tex ) {
				// tex = stage->texture;
			// }
			// if( tex ) {
				// tex->bind( device, i );
			// } else {
				// checkerror( device->SetTexture( i, 0 ) );
			// }
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_CONSTANT, stage->constant ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_COLOROP, stage->colorop ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_COLORARG1, stage->colorarg1 ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_COLORARG2, stage->colorarg2 ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_ALPHAOP, stage->alphaop ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_ALPHAARG1, stage->alphaarg1 ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_ALPHAARG2, stage->alphaarg2 ) );
			// checkerror( device->SetTextureStageState(
				// i, D3DTSS_TEXCOORDINDEX, stage->texcoord ) );
			// if( stage->matrix ) {
				// checkerror( device->SetTextureStageState(
					// i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED ) );
				// checkerror( device->SetTransform(
					// D3DTRANSFORMSTATETYPE( int( D3DTS_TEXTURE0 ) + i ),
					// stage->matrix ) );
			// } else {
				// checkerror( device->SetTextureStageState(
					// i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE ) );
			// }
		// }
		if( !m_vertexshader || !m_pixelshader )
		{
			return;
		}
		checkerror( device->SetVertexShader( m_vertexshader ) );
		checkerror( device->SetPixelShader( m_pixelshader ) );
		Stage* stage = &m_stages[ 0 ];
		Texture* tex = textures[ 0 ];
		if( !tex )
		{
			tex = stage->texture;
		}
		if( tex )
		{
			tex->bind( device, 0 );
		}
		else
		{
			checkerror( device->SetTexture( 0, 0 ) );
		}
	}

	void PixelProgram::setstagetexture( int stage, Texture* texture ) {
		TABLE_ASSERT( m_stages, stage, "stage" ).texture = texture;
	}

	void PixelProgram::setstageconstant( int stage, float const* constant ) {
		TABLE_ASSERT( m_stages, stage, "stage" ).constant = argb8( constant );
	}

	void PixelProgram::setstagecolor( int stage, int op, int arg1, int arg2 ) {
		TABLE_ASSERT( m_stages, stage, "stage" ).colorop =
			TABLE_ASSERT( optable, op, "operator" );
		TABLE_ASSERT( m_stages, stage, "stage" ).colorarg1 =
			TABLE_ASSERT( argtable, arg1, "argument" );
		TABLE_ASSERT( m_stages, stage, "stage" ).colorarg2 =
			TABLE_ASSERT( argtable, arg2, "argument" );
	}

	void PixelProgram::setstagealpha( int stage, int op, int arg1, int arg2 ) {
		TABLE_ASSERT( m_stages, stage, "stage" ).alphaop =
			TABLE_ASSERT( optable, op, "operator" );
		TABLE_ASSERT( m_stages, stage, "stage" ).alphaarg1 =
			TABLE_ASSERT( argtable, arg1, "argument" );
		TABLE_ASSERT( m_stages, stage, "stage" ).alphaarg2 =
			TABLE_ASSERT( argtable, arg2, "argument" );
	}

	void PixelProgram::setstagetexcoord( int stage, int index ) {
		TABLE_ASSERT( m_stages, stage, "stage" ).texcoord =
			TABLE_ASSERT( texcoordtable, index, "texture coordinate index" );
	}

	void PixelProgram::setstagematrix( int stage, int index ) {
		Stage* stagep = &TABLE_ASSERT( m_stages, stage, "stage" );
		RANGE_ASSERT( index, Matrix_Identity, Matrix_Invalid, "matrix index" );
		if( index == Matrix_Identity ) {
			stagep->matrix = 0;
		} else {
			stagep->matrix = m_matrices + ( index - Matrix_Mat1 );
		}
	}

	void PixelProgram::setmatrix( int index, float const* data ) {
		RANGE_ASSERT( index, Matrix_Mat1, Matrix_Invalid, "matrix index" );
		D3DMATRIX* matrix = m_matrices + ( index - Matrix_Mat1 );
		bool inverse = data[ 11 ] < 0;
		for( int row = 0; row < 4; ++row ) {
			for( int col = 0; col < 4; ++col ) {
				matrix->m[ row ][ col ] =
					inverse ?
					( - data[ ( row << 2 ) + col ] ) :
					data[ ( row << 2 ) + col ];
			}
		}
	}

	extern "C" {
		PixelProgram* graphics_pixelprogram_new() noexcept {
		CBASE_PROTECT(
			return new PixelProgram();
		)
		}

		bool graphics_pixelprogram_setstagetexture( PixelProgram* pp, int stage, Texture* texture ) noexcept {
		CBASE_PROTECT(
			pp->setstagetexture( stage, texture );
			return 1;
		)
		}

		bool graphics_pixelprogram_setstageconstant( PixelProgram* pp, int stage, float const* constant ) noexcept {
		CBASE_PROTECT(
			pp->setstageconstant( stage, constant );
			return 1;
		)
		}

		bool graphics_pixelprogram_setstagecolor( PixelProgram* pp, int stage, int op, int arg1, int arg2 ) noexcept {
		CBASE_PROTECT(
			pp->setstagecolor( stage, op, arg1, arg2 );
			return 1;
		)
		}

		bool graphics_pixelprogram_setstagealpha( PixelProgram* pp, int stage, int op, int arg1, int arg2 ) noexcept {
		CBASE_PROTECT(
			pp->setstagealpha( stage, op, arg1, arg2 );
			return 1;
		)
		}

		bool graphics_pixelprogram_setstagetexcoord( PixelProgram* pp, int stage, int index ) noexcept {
		CBASE_PROTECT(
			pp->setstagetexcoord( stage, index );
			return 1;
		)
		}

		bool graphics_pixelprogram_setstagematrix( PixelProgram* pp, int stage, int index ) noexcept {
		CBASE_PROTECT(
			pp->setstagematrix( stage, index );
			return 1;
		)
		}

		bool graphics_pixelprogram_setmatrix( PixelProgram* pp, int index, float const* data ) noexcept {
		CBASE_PROTECT(
			pp->setmatrix( index, data );
			return 1;
		)
		}
	}
}
