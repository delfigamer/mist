#include "rendertarget.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	void RenderTarget::update( IDirect3DDevice9* device ) {
		if( !m_texture ) {
			checkerror( device->CreateTexture(
				m_width, m_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texture, 0 ) );
		}
		if( m_dsuse ) {
			if( !m_dssurface ) {
				checkerror( device->CreateDepthStencilSurface(
					m_width, m_height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &m_dssurface, 0 ) );
			}
		} else {
			if( m_dssurface ) {
				RELEASE( m_dssurface );
			}
		}
		Shape* shape = m_shape;
		if( shape ) {
			shape->advance( device, m_lastframe );
			IDirect3DSurface9* targetsurface;
			IDirect3DSurface9* colorsurface;
			IDirect3DSurface9* dssurface;
			checkerror( m_texture->GetSurfaceLevel(
				0, &targetsurface ) );
			checkerror( device->GetRenderTarget( 0, &colorsurface ) );
			checkerror( device->SetRenderTarget( 0, targetsurface ) );
			try {
				checkerror( device->GetDepthStencilSurface( &dssurface ) );
				checkerror( device->SetDepthStencilSurface( m_dssurface ) );
				try {
					if( m_clearflags ) {
						checkerror( device->Clear(
							0, 0, m_clearflags, m_clearcolor, m_cleardepth, m_clearstencil ) );
					}
					shape->paint( device );
				} catch( ... ) {
					device->SetDepthStencilSurface( dssurface );
				}
				checkerror( device->SetDepthStencilSurface( dssurface ) );
			} catch( ... ) {
				device->SetRenderTarget( 0, colorsurface );
			}
			checkerror( device->SetRenderTarget( 0, colorsurface ) );
		}
	}

	RenderTarget::RenderTarget( int width, int height ) :
		m_dssurface( 0 ) ,
		m_width( width ) ,
		m_height( height ) ,
		m_clearflags( 0 ) {
	}

	RenderTarget::~RenderTarget() {
		RELEASE( m_dssurface );
	}

	void RenderTarget::setdepthstenciluse( bool use ) {
		m_dsuse = use;
	}

	void RenderTarget::setshape( Shape* shape ) {
		m_shape = shape;
	}

	void RenderTarget::setclearcolor( bool flag, float const* value ) {
		if( flag ) {
			m_clearflags |= D3DCLEAR_TARGET;
			m_clearcolor = argb8( value );
		} else {
			m_clearflags &= ~D3DCLEAR_TARGET;
		}
	}

	void RenderTarget::setcleardepth( bool flag, float value ) {
		if( flag ) {
			m_clearflags |= D3DCLEAR_ZBUFFER;
			m_cleardepth = value;
		} else {
			m_clearflags &= ~D3DCLEAR_ZBUFFER;
		}
	}

	void RenderTarget::setclearstencil( bool flag, int value ) {
		if( flag ) {
			m_clearflags |= D3DCLEAR_STENCIL;
			m_clearstencil = value;
		} else {
			m_clearflags &= ~D3DCLEAR_STENCIL;
		}
	}

	extern "C" {
		RenderTarget* graphics_rendertarget_new( int width, int height ) noexcept {
		CBASE_PROTECT(
			return new RenderTarget( width, height );
		)
		}

		bool graphics_rendertarget_setdepthstenciluse( RenderTarget* rt, bool use ) noexcept {
		CBASE_PROTECT(
			rt->setdepthstenciluse( use );
			return 1;
		)
		}

		bool graphics_rendertarget_setshape( RenderTarget* rt, Shape* shape ) noexcept {
		CBASE_PROTECT(
			rt->setshape( shape );
			return 1;
		)
		}

		bool graphics_rendertarget_setclearcolor( RenderTarget* rt, bool flag, float const* value ) noexcept {
		CBASE_PROTECT(
			rt->setclearcolor( flag, value );
			return 1;
		)
		}

		bool graphics_rendertarget_setcleardepth( RenderTarget* rt, bool flag, float value ) noexcept {
		CBASE_PROTECT(
			rt->setcleardepth( flag, value );
			return 1;
		)
		}

		bool graphics_rendertarget_setclearstencil( RenderTarget* rt, bool flag, int value ) noexcept {
		CBASE_PROTECT(
			rt->setclearstencil( flag, value );
			return 1;
		)
		}
	}
}
