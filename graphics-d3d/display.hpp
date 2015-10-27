#ifndef GRAPHICS_DISPLAY_HPP__
#define GRAPHICS_DISPLAY_HPP__ 1

#if !defined( _WIN32 )
#error "Unsupported target platform"
#endif

#include "shape.hpp"
#include <utils/ref.hpp>
#include <d3d9.h>
#include <windows.h>
#include <ctime>

namespace graphics {
	struct DisplayInfo {
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
	};
	
	class Display {
	private:
		HWND m_hwnd;
		D3DPRESENT_PARAMETERS m_presentparameters;
		IDirect3D9* m_direct3d;
		IDirect3DDevice9* m_device;
		utils::Ref< graphics::Shape > m_shape;
		DisplayInfo m_info;
		int m_framecounter;
		
	public:
		Display();
		~Display();
		Display( Display const& ) = delete;
		Display( Display&& ) = delete;
		Display& operator=( Display const& ) = delete;
		Display& operator=( Display&& ) = delete;
		
		void initialize( HWND hwnd );
		void finalize();
		void paint();
		Shape* getshape();
		void setshape( Shape* nv );
		DisplayInfo const* displayinfo();
	};
}

#endif
