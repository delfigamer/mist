#pragma once

#if !defined( _WIN32 ) && !defined( _WIN64 )
#error "Unsupported target platform"
#endif

#include <renderer-gles/shape.hpp>
#include <utils/ref.hpp>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <osapi.hpp>

namespace graphics
{
	class Display
	{
	private:
		HWND m_hwnd;
		HDC m_hdc;
		EGLDisplay m_display;
		EGLSurface m_surface;
		EGLContext m_context;
		utils::Ref< graphics::Shape > m_shape;

		void initialize();
		void finalize();

	public:
		Display( HWND hwnd );
		~Display();
		Display( Display const& ) = delete;
		Display& operator=( Display const& ) = delete;

		void paint();
		void setshape( Shape* nv );
	};
}
