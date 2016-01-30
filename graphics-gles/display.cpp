#include "display.hpp"
#include "shape.hpp"
#include "resource.hpp"
#include "common.hpp"
#include <utils/configset.hpp>
#include <utils/profile.hpp>
#include <utils/console.hpp>
#include <GLES2/gl2.h>
#include <stdexcept>

namespace graphics
{
	Display::Display()
		: m_app( 0 )
		, m_display( EGL_NO_DISPLAY )
		, m_surface( EGL_NO_SURFACE )
		, m_context( EGL_NO_CONTEXT )
		, m_info{ 0, 0, 0, 0 }
		, m_framecounter( 0 )
	{
	}

	Display::~Display()
	{
	}

	void Display::initialize( utils::ConfigSet const& config, android_app* app )
	{
		if( m_app )
		{
			return;
		}
		m_app = app;
		static EGLint const attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
//			EGL_DEPTH_SIZE, 16,
			EGL_NONE,
		};
		EGLint format;
		EGLint numConfigs;
		EGLConfig dconfig;
		m_display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
		eglInitialize( m_display, 0, 0 );
		eglChooseConfig(
			m_display, attribs, &dconfig, 1, &numConfigs );
		eglGetConfigAttrib(
			m_display, dconfig, EGL_NATIVE_VISUAL_ID, &format );
		ANativeWindow_setBuffersGeometry(
			m_app->window, 0, 0, format );
		m_surface = eglCreateWindowSurface(
			m_display, dconfig, m_app->window, 0 );
		m_context = eglCreateContext( m_display, dconfig, 0, 0 );
		if( eglMakeCurrent(
			m_display, m_surface, m_surface, m_context ) ==
			EGL_FALSE )
		{
			throw std::runtime_error(
				"failed to initialize a display" );
		}
		eglQuerySurface(
			m_display, m_surface, EGL_WIDTH, &m_info.width );
		eglQuerySurface(
			m_display, m_surface, EGL_HEIGHT, &m_info.height );
		glDisable( GL_CULL_FACE );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	void Display::finalize()
	{
		if( m_display != EGL_NO_DISPLAY )
		{
			eglMakeCurrent(
				m_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
				EGL_NO_CONTEXT );
			if( m_context != EGL_NO_CONTEXT )
			{
				eglDestroyContext( m_display, m_context );
				m_context = EGL_NO_CONTEXT;
			}
			if( m_surface != EGL_NO_SURFACE)
			{
				eglDestroySurface( m_display, m_surface );
				m_surface = EGL_NO_SURFACE;
			}
			eglTerminate( m_display );
			m_display = EGL_NO_DISPLAY;
		}
		m_app = 0;
	}

	void Display::paint()
	{
	PROFILE( "paint",
		if( m_display == EGL_NO_DISPLAY )
		{
			return;
		}
		++m_framecounter;
		utils::Ref< graphics::Shape > shape = m_shape;
		if( shape )
		{
		PROFILE( "paint shape->advance",
			shape->advance( m_framecounter );
		)
		PROFILE( "paint shape->paint",
			shape->paint();
		)
		}
		else
		{
		PROFILE( "paint glClear",
			glClearColor( 0.3, 0.3, 0.3, 1 );
			checkerror();
			glClear( GL_COLOR_BUFFER_BIT );
			checkerror();
		)
		}
	PROFILE( "paint eglSwapBuffers",
		eglSwapBuffers( m_display, m_surface );
	)
	PROFILE( "paint graphics::Resource::cleanup",
		graphics::Resource::cleanup();
	)
	)
	}

	Shape* Display::getshape()
	{
		return m_shape;
	}

	void Display::setshape( Shape* nv )
	{
		m_shape = nv;
	}

	DisplayInfo const* Display::displayinfo()
	{
		return &m_info;
	}
}
