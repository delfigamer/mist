#include "texture.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <cmath>

namespace graphics
{
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
		D3DTADDRESS_CLAMP,
		D3DTADDRESS_MIRROR,
	};

	Texture::Texture()
		: m_texture( 0 )
		, m_minfilter( D3DTEXF_LINEAR )
		, m_mipfilter( D3DTEXF_LINEAR )
		, m_magfilter( D3DTEXF_LINEAR )
		, m_wrapu( D3DTADDRESS_WRAP )
		, m_wrapv( D3DTADDRESS_WRAP )
	{
	}

	Texture::~Texture()
	{
		RELEASE( m_texture );
	}

	bool Texture::bind( int sampler )
	{
		if( m_texture )
		{
			checkerror( Context::Device->SetTexture( sampler, m_texture ) );
			checkerror( Context::Device->SetSamplerState(
				sampler, D3DSAMP_ADDRESSU, m_wraps ) );
			checkerror( Context::Device->SetSamplerState(
				sampler, D3DSAMP_ADDRESSV, m_wrapt ) );
			checkerror( Context::Device->SetSamplerState(
				sampler, D3DSAMP_MAGFILTER, m_magfilter ) );
			checkerror( Context::Device->SetSamplerState(
				sampler, D3DSAMP_MINFILTER, m_minfilter ) );
			checkerror( Context::Device->SetSamplerState(
				sampler, D3DSAMP_MIPFILTER, m_mipfilter ) );
		}
		else
		{
			checkerror( Context::Device->SetTexture( sampler, 0 ) );
		}
		return true;
	}

	void Texture::setminfilter( int value )
	{
		m_minfilter = TABLE_ASSERT(
			minfiltertable, value, "minification filter" );
		m_mipfilter = TABLE_ASSERT(
			mipfiltertable, value, "minification filter" );
	}

	void Texture::setmagfilter( int value )
	{
		m_magfilter = TABLE_ASSERT(
			magfiltertable, value, "magnification filter" );
	}

	void Texture::setwrapmode( int wu, int wv )
	{
		wu = TABLE_ASSERT( wrapmodetable, wu, "wrap mode" );
		wv = TABLE_ASSERT( wrapmodetable, wv, "wrap mode" );
		m_wrapu = wu;
		m_wrapv = wv;
	}

	bool graphics_texture_setminfilter( Texture* t, int value ) noexcept
	{
		CBASE_PROTECT(
			t->setminfilter( value );
			return 1;
		)
	}

	bool graphics_texture_setmagfilter( Texture* t, int value ) noexcept
	{
		CBASE_PROTECT(
			t->setmagfilter( value );
			return 1;
		)
	}

	bool graphics_texture_setwrapmode( Texture* t, int wu, int wv ) noexcept
	{
		CBASE_PROTECT(
			t->setwrapmode( wu, wv );
			return 1;
		)
	}
}
