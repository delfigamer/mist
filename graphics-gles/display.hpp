#ifndef GRAPHICS_DISPLAY_HPP__
#define GRAPHICS_DISPLAY_HPP__ 1

#if !defined( __ANDROID__ )
#error "Unsupported target platform"
#endif

// #include "shape.hpp"
#include <utils/ref.hpp>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <ctime>

namespace graphics
{
	class Shape: public utils::RefObject {};

	struct DisplayInfo
	{
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
	};
	
	class Display
	{
	private:
		android_app* m_app;
		EGLDisplay m_display;
		EGLSurface m_surface;
		EGLContext m_context;
		utils::Ref< graphics::Shape > m_shape;
		DisplayInfo m_info;
		int m_framecounter;
		
	public:
		Display();
		~Display();
		Display( Display& ) = delete;
		Display& operator=( Display& ) = delete;
		
		void initialize( android_app* app );
		void finalize();
		void paint();
		Shape* getshape();
		void setshape( Shape* nv );
		DisplayInfo const* displayinfo();
	};
}

#endif
