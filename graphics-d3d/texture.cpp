#include "texture.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>
#include <cmath>
#include <cstdio>

namespace graphics {
	static int const minfiltertable[] = {
		D3DTEXF_POINT,
		D3DTEXF_LINEAR,
		D3DTEXF_POINT,
		D3DTEXF_LINEAR,
		D3DTEXF_POINT,
		D3DTEXF_LINEAR
	};
	static int const mipfiltertable[] = {
		D3DTEXF_NONE,
		D3DTEXF_NONE,
		D3DTEXF_POINT,
		D3DTEXF_POINT,
		D3DTEXF_LINEAR,
		D3DTEXF_LINEAR
	};
	static int const magfiltertable[] = {
		D3DTEXF_POINT,
		D3DTEXF_LINEAR
	};
	static int const wrapmodetable[] = {
		D3DTADDRESS_WRAP,
		D3DTADDRESS_CLAMP
	};

	void Texture::doadvance( IDirect3DDevice9* device, int framecount ) {
		update( device );
	}

	Texture::Texture() :
		m_texture( 0 ) ,
		m_minfilter( D3DTEXF_LINEAR ) ,
		m_mipfilter( D3DTEXF_LINEAR ) ,
		m_magfilter( D3DTEXF_LINEAR ) ,
		m_wraps( D3DTADDRESS_WRAP ) ,
		m_wrapt( D3DTADDRESS_WRAP ) {
	}

	Texture::~Texture() {
		RELEASE( m_texture );
	}

	bool Texture::istexture() {
		return m_texture;
	}

	void Texture::bind( IDirect3DDevice9* device, int sampler ) {
		if( m_texture ) {
			checkerror( device->SetTexture( sampler, m_texture ) );
			checkerror( device->SetSamplerState( sampler, D3DSAMP_ADDRESSU, m_wraps ) );
			checkerror( device->SetSamplerState( sampler, D3DSAMP_ADDRESSV, m_wrapt ) );
			checkerror( device->SetSamplerState( sampler, D3DSAMP_MAGFILTER, m_magfilter ) );
			checkerror( device->SetSamplerState( sampler, D3DSAMP_MINFILTER, m_minfilter ) );
			checkerror( device->SetSamplerState( sampler, D3DSAMP_MIPFILTER, m_mipfilter ) );
		} else {
			checkerror( device->SetTexture( sampler, 0 ) );
		}
	}

// 	void Texture::unbind() {
// 	}

	void Texture::setminfilter( int value ) {
		if( value < 0 || value >= MinFilter_Invalid ) {
			throw std::runtime_error( "unknown min filter value" );
		}
		m_minfilter = minfiltertable[ value ];
		m_mipfilter = mipfiltertable[ value ];
	}

	void Texture::setmagfilter( int value ) {
		if( value < 0 || value >= MagFilter_Invalid ) {
			throw std::runtime_error( "unknown mag filter value" );
		}
		m_magfilter = magfiltertable[ value ];
	}

	void Texture::setwrapmode( int ws, int wt ) {
		if( ws < 0 || ws >= WrapMode_Invalid ||
			wt < 0 || wt >= WrapMode_Invalid ) {
			throw std::runtime_error( "unknown wrap mode value" );
		}
		m_wraps = wrapmodetable[ ws ];
		m_wrapt = wrapmodetable[ wt ];
	}

	extern "C" {
		bool graphics_texture_setminfilter( Texture* t, int value ) noexcept {
			CBASE_PROTECT(
				t->setminfilter( value );
				return 1;
			)
		}

		bool graphics_texture_setmagfilter( Texture* t, int value ) noexcept {
			CBASE_PROTECT(
				t->setmagfilter( value );
				return 1;
			)
		}

		bool graphics_texture_setwrapmode( Texture* t, int ws, int wt ) noexcept {
			CBASE_PROTECT(
				t->setwrapmode( ws, wt );
				return 1;
			)
		}
	}
}
