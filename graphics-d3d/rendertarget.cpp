#include "rendertarget.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics
{
	static int const texformattable[] = {
		D3DFMT_A8R8G8B8,
		D3DFMT_A8R8G8B8,
	};
	static int const dsformattable[] = {
		-1,
		D3DFMT_D24S8,
	};

	void RenderTarget::advance()
	{
		if( !m_texture )
		{
			checkerror( device->CreateTexture(
				m_width, m_height, 1, D3DUSAGE_RENDERTARGET, m_texformat,
				D3DPOOL_DEFAULT, &m_texture, 0 ) );
			if( m_dsformat != -1 )
			{
				checkerror( device->CreateDepthStencilSurface(
					m_width, m_height, m_dsformat, D3DMULTISAMPLE_NONE,
					0, true, &m_dssurface, 0 ) );
			}
		}
		Shape* shape = m_shape;
		if( shape )
		{
			shape->advance();
			IDirect3DSurface9* targetsurface;
			IDirect3DSurface9* colorsurface;
			IDirect3DSurface9* dssurface;
			checkerror( m_texture->GetSurfaceLevel( 0, &targetsurface ) );
			checkerror( device->GetRenderTarget( 0, &colorsurface ) );
			checkerror( device->SetRenderTarget( 0, targetsurface ) );
			try
			{
				checkerror( device->GetDepthStencilSurface( &dssurface ) );
				checkerror( device->SetDepthStencilSurface( m_dssurface ) );
				try
				{
					shape->paint();
				}
				catch( ... )
				{
					device->SetDepthStencilSurface( dssurface );
				}
				checkerror( device->SetDepthStencilSurface( dssurface ) );
			}
			catch( ... )
			{
				device->SetRenderTarget( 0, colorsurface );
			}
			checkerror( device->SetRenderTarget( 0, colorsurface ) );
		}
	}

	RenderTarget::RenderTarget( int format, int width, int height )
		: m_dssurface( 0 )
		, m_width( width )
		, m_height( height )
	{
		m_texformat = TABLE_ASSERT( texformattable, format, "format" );
		m_dsformat = TABLE_ASSERT( dsformattable, format, "format" );
	}

	RenderTarget::~RenderTarget()
	{
		RELEASE( m_dssurface );
	}

	void RenderTarget::setshape( Shape* shape )
	{
		m_shape = shape;
	}

	RenderTarget* graphics_rendertarget_new(
		int format, int width, int height ) noexcept
	{
	CBASE_PROTECT(
		return new RenderTarget( format, width, height );
	)
	}

	bool graphics_rendertarget_setshape(
		RenderTarget* rt, Shape* shape ) noexcept
	{
	CBASE_PROTECT(
		rt->setshape( shape );
		return 1;
	)
	}
}
