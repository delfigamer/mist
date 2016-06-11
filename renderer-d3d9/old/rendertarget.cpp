#include <renderer-d3d9/rendertarget.hpp>
#include <renderer-d3d9/common.hpp>
#include <renderer-d3d9/context.hpp>
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

	void RenderTarget::doadvance()
	{
		if( !m_texture )
		{
			checkerror( Context::Device->CreateTexture(
				m_width, m_height, 1, D3DUSAGE_RENDERTARGET,
				D3DFORMAT( m_texformat ),
				D3DPOOL_DEFAULT, &m_texture, 0 ) );
			if( m_dsformat != -1 )
			{
				checkerror( Context::Device->CreateDepthStencilSurface(
					m_width, m_height, D3DFORMAT( m_dsformat ), D3DMULTISAMPLE_NONE,
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
			checkerror( Context::Device->GetRenderTarget( 0, &colorsurface ) );
			checkerror( Context::Device->SetRenderTarget( 0, targetsurface ) );
			try
			{
				checkerror( Context::Device->GetDepthStencilSurface( &dssurface ) );
				checkerror( Context::Device->SetDepthStencilSurface(
					m_dssurface ) );
				try
				{
					shape->paint();
				}
				catch( ... )
				{
					Context::Device->SetDepthStencilSurface( dssurface );
				}
				checkerror( Context::Device->SetDepthStencilSurface( dssurface ) );
			}
			catch( ... )
			{
				Context::Device->SetRenderTarget( 0, colorsurface );
			}
			checkerror( Context::Device->SetRenderTarget( 0, colorsurface ) );
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

	bool RenderTarget::isformatsupported( int format ) noexcept
	{
		// return format >= 0 && format < RTFormat_Invalid;
		return format == RTFormat_RGBA8;
	}
}
