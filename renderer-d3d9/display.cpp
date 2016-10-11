#include <renderer-d3d9/display.hpp>
#include <renderer-d3d9/resource.hpp>
#include <renderer-d3d9/context.hpp>
#include <renderer-d3d9/common.hpp>
#include <renderer-d3d9/interface.hpp>
#include <stdexcept>
#include <ctime>

namespace graphics
{
	Display::Display( HWND hwnd )
		: m_hwnd( hwnd )
	{
		D3DPRESENT_PARAMETERS presentparameters = {
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
			getconfig_boolean( "vsync", false ) ?
				D3DPRESENT_INTERVAL_ONE :
				D3DPRESENT_INTERVAL_IMMEDIATE,
		};
		Context::D3D.possess( Direct3DCreate9( D3D_SDK_VERSION ) );
		if( !Context::D3D )
		{
			throw std::runtime_error( "Direct3DCreate9 failed" );
		}
		IDirect3DDevice9* device = 0;
		checkerror( Context::D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&presentparameters, &device ) );
		Context::Device.possess( device );
		WindowInfo->width = presentparameters.BackBufferWidth;
		WindowInfo->height = presentparameters.BackBufferHeight;
		WindowInfo->texelsoffset = 0.5;
		WindowInfo->texeltoffset = -0.5;
		checkerror( Context::Device->SetRenderState(
			D3DRS_CULLMODE, D3DCULL_NONE ) );
		checkerror( Context::Device->SetRenderState( D3DRS_LIGHTING, false ) );
	}

	Display::~Display()
	{
		m_shape = nullptr;
		Context::rundeferred();
		Context::Device = nullptr;
		Context::D3D = nullptr;
	}

	void Display::paint()
	{
		if( !Context::Device )
		{
			return;
		}
		Ref< graphics::Shape > shape = m_shape;
		if( shape )
		{
			Context::DrawnFrame += 1;
			checkerror( Context::Device->BeginScene() );
			shape->advance();
			shape->paint();
			checkerror( Context::Device->EndScene() );
		}
		else
		{
			checkerror( Context::Device->Clear(
				0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
				0xff4c4c4c, 0, 0 ) );
		}
		checkerror( Context::Device->Present( 0, 0, 0, 0 ) );
		Context::rundeferred();
	}

	void Display::setshape( Shape* nv )
	{
		m_shape = nv;
	}
}
