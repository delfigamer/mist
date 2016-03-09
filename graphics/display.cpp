#include <graphics/display.hpp>
#include <graphics/resource.hpp>
#include <graphics/context.hpp>
#include <graphics/common.hpp>
#include <utils/profile.hpp>
#include <stdexcept>
#include <ctime>

namespace graphics
{
	Display::Display()
		: m_hwnd( 0 )
		, m_info{ 0, 0, 0.5, -0.5 }
	{
	}

	Display::~Display()
	{
	}

	void Display::initialize( utils::ConfigSet const& config, HWND hwnd )
	{
		if( m_hwnd )
		{
			return;
		}
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
			config.boolean( "vsync" ) ?
				D3DPRESENT_INTERVAL_ONE :
				D3DPRESENT_INTERVAL_IMMEDIATE,
		};
		m_hwnd = hwnd;
		Context::D3D = Direct3DCreate9( D3D_SDK_VERSION );
		if( !Context::D3D )
		{
			throw std::runtime_error( "Direct3DCreate9 failed" );
		}
		checkerror( Context::D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&presentparameters, &Context::Device ) );
		m_info.width = presentparameters.BackBufferWidth;
		m_info.height = presentparameters.BackBufferHeight;
		checkerror( Context::Device->SetRenderState(
			D3DRS_CULLMODE, D3DCULL_NONE ) );
		checkerror( Context::Device->SetRenderState( D3DRS_LIGHTING, false ) );
	}

	void Display::finalize()
	{
		RELEASE( Context::Device );
		RELEASE( Context::D3D );
		m_hwnd = 0;
	}

	void Display::paint()
	{
	PROFILE( "paint",
		if( !Context::Device )
		{
			return;
		}
		utils::Ref< graphics::Shape > shape = m_shape;
		if( shape )
		{
			Context::DrawnFrame += 1;
		PROFILE( "paint Context::Device->BeginScene",
			checkerror( Context::Device->BeginScene() );
		)
		PROFILE( "paint shape->advance",
			shape->advance();
		)
		PROFILE( "paint shape->paint",
			shape->paint();
		)
		PROFILE( "paint Context::Device->EndScene",
			checkerror( Context::Device->EndScene() );
		)
		}
		else
		{
		PROFILE( "paint Context::Device->Clear",
			checkerror( Context::Device->Clear(
				0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
				0xff4c4c4c, 0, 0 ) );
		)
		}
	PROFILE( "paint Context::Device->Present",
		checkerror( Context::Device->Present( 0, 0, 0, 0 ) );
	)
	PROFILE( "paint Context::cleanup",
		Context::cleanup();
	)
	)
	}

	DisplayInfo const* Display::getdisplayinfo()
	{
		return &m_info;
	}

	void Display::setshape( Shape* nv )
	{
		m_shape = nv;
	}
}
