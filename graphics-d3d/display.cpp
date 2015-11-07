#include "display.hpp"
#include "resource.hpp"
#include "common.hpp"
#include <utils/profile.hpp>
#include <stdexcept>

namespace graphics {
	Display::Display() :
		m_hwnd( 0 ) ,
		m_presentparameters {
			0,
			0,
			D3DFMT_A8R8G8B8,
			0,
			D3DMULTISAMPLE_NONE,
			0,
			D3DSWAPEFFECT_DISCARD,
			0,
			true,
			true,
			D3DFMT_D24S8,
			0,
			0,
			D3DPRESENT_INTERVAL_IMMEDIATE },
		m_direct3d( 0 ) ,
		m_device( 0 ) ,
		m_info{ 0, 0, 0.5, -0.5 } ,
		m_framecounter( 0 ) {
	}

	Display::~Display() {
	}

	void Display::initialize( utils::ConfigSet const& config, HWND hwnd ) {
		if( m_hwnd ) {
			return;
		}
		if( config.boolean( "vsync" ) )
		{
			m_presentparameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		m_hwnd = hwnd;
		m_direct3d = Direct3DCreate9( D3D_SDK_VERSION );
		if( !m_direct3d ) {
			throw std::runtime_error( "Direct3DCreate9 failed" );
		}
		checkerror( m_direct3d->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_presentparameters, &m_device ) );
		m_info.width = m_presentparameters.BackBufferWidth;
		m_info.height = m_presentparameters.BackBufferHeight;
		checkerror( m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) );
		checkerror( m_device->SetRenderState( D3DRS_LIGHTING, false ) );
		checkerror( m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
		checkerror( m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );
		checkerror( m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true ) );
	}

	void Display::finalize() {
		RELEASE( m_device );
		RELEASE( m_direct3d );
		m_hwnd = 0;
	}

	void Display::paint() {
	PROFILE( "paint",
		if( !m_device ) {
			return;
		}
		utils::Ref< graphics::Shape > shape = m_shape;
		if( shape ) {
		PROFILE( "paint m_device->BeginScene",
			checkerror( m_device->BeginScene() );
		)
		PROFILE( "paint shape->advance",
			shape->advance( m_device, ++m_framecounter );
		)
		PROFILE( "paint shape->paint",
			shape->paint( m_device );
		)
		PROFILE( "paint m_device->EndScene",
			checkerror( m_device->EndScene() );
		)
		} else {
		PROFILE( "paint m_device->Clear",
			checkerror( m_device->Clear(
				0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
				0xff4c4c4c, 0, 0 ) );
		)
		}
	PROFILE( "paint m_device->Present",
		checkerror( m_device->Present( 0, 0, 0, 0 ) );
	)
	PROFILE( "paint graphics::Resource::cleanup",
		graphics::Resource::cleanup();
	)
	)
	}

	Shape* Display::getshape() {
		return m_shape;
	}

	void Display::setshape( Shape* nv ) {
		m_shape = nv;
	}

	DisplayInfo const* Display::displayinfo() {
		return &m_info;
	}
}
