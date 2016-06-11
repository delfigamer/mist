#include <renderer-gles/display.hpp>
#include <renderer-gles/resource.hpp>
#include <renderer-gles/context.hpp>
#include <renderer-gles/common.hpp>
#include <renderer-gles/interface.hpp>
#include <renderer-gles/gl2.hpp>
#include <stdexcept>
#include <cstring>
#include <ctime>

namespace graphics
{
	void Display::initialize()
	{
		m_hdc = GetDC( m_hwnd );
		if( !m_hdc )
		{
			syserror();
		}

		auto eglGetPlatformDisplayEXT = PFNEGLGETPLATFORMDISPLAYEXTPROC(
			eglGetProcAddress( "eglGetPlatformDisplayEXT" ) );
		if( !eglGetPlatformDisplayEXT )
		{
			throw std::runtime_error(
				"eglGetPlatformDisplayEXT is not available" );
		}

		EGLint displayAttributes[] = {
			EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE,
			EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, -1,
			EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, -1,
			EGL_NONE,
		};

		m_display = eglGetPlatformDisplayEXT(
			EGL_PLATFORM_ANGLE_ANGLE, m_hdc, displayAttributes );
		if( m_display == EGL_NO_DISPLAY )
		{
			throw std::runtime_error( "failed to create an EGL display" );
		}

		EGLint majorVersion, minorVersion;
		if( eglInitialize(
			m_display, &majorVersion, &minorVersion ) == EGL_FALSE )
		{
			throw std::runtime_error( "failed to initialize an EGL display" );
		}

		const char *displayExtensions = eglQueryString(
			m_display, EGL_EXTENSIONS );

		// EGL_KHR_create_context is required to request a non-ES2 context.
		bool hasKHRCreateContext = strstr(
			displayExtensions, "EGL_KHR_create_context" ) != 0;
		if( majorVersion != 2 && minorVersion != 0 && !hasKHRCreateContext )
		{
			throw std::runtime_error( "EGL_KHR_create_context is not available" );
		}

		eglBindAPI( EGL_OPENGL_ES_API );
		if( eglGetError() != EGL_SUCCESS )
		{
			throw std::runtime_error( "failed to bind the GL ES API" );
		}

		const EGLint configAttributes[] = {
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, EGL_DONT_CARE,
			EGL_STENCIL_SIZE, EGL_DONT_CARE,
			// EGL_SAMPLE_BUFFERS, mMultisample ? 1 : 0,
			EGL_SAMPLE_BUFFERS, 0,
			EGL_NONE,
		};

		EGLint configCount;
		EGLConfig config;
		if( !eglChooseConfig(
				m_display, configAttributes, &config, 1, &configCount ) ||
			( configCount != 1 ) )
		{
			throw std::runtime_error( "failed to choose an EGL config" );
		}

		EGLint surfaceAttributes[] = {
			EGL_POST_SUB_BUFFER_SUPPORTED_NV, EGL_TRUE,
			EGL_NONE,
		};
		if( strstr( displayExtensions, "EGL_NV_post_sub_buffer" ) == nullptr )
		{
			surfaceAttributes[ 0 ] = EGL_NONE;
		}

		m_surface = eglCreateWindowSurface(
			m_display, config, m_hwnd, surfaceAttributes );
		if( eglGetError() != EGL_SUCCESS || m_surface == EGL_NO_SURFACE )
		{
			throw std::runtime_error( "failed to create an EGL surface" );
		}

		EGLint contextAttributes[] = {
			EGL_CONTEXT_MAJOR_VERSION_KHR, 2,
			EGL_CONTEXT_MINOR_VERSION_KHR, 0,
			// EGL_CONTEXT_OPENGL_DEBUG, mDebug ? EGL_TRUE : EGL_FALSE,
			EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
			// EGL_CONTEXT_OPENGL_NO_ERROR_KHR, mNoError ? EGL_TRUE : EGL_FALSE,
			EGL_CONTEXT_OPENGL_NO_ERROR_KHR, EGL_FALSE,
			EGL_NONE,
		};

		m_context = eglCreateContext(
			m_display, config, nullptr, contextAttributes );
		if( eglGetError() != EGL_SUCCESS )
		{
			throw std::runtime_error( "failed to create an EGL context" );
		}

		eglMakeCurrent( m_display, m_surface, m_surface, m_context);
		if( eglGetError() != EGL_SUCCESS )
		{
			throw std::runtime_error( "failed to set current EGL context" );
		}

		eglSwapInterval( m_display, getconfig_boolean( "vsync" ) ? 1 : 0 );

		eglQuerySurface( m_display, m_surface, EGL_WIDTH, &WindowInfo->width );
		eglQuerySurface( m_display, m_surface, EGL_HEIGHT, &WindowInfo->height );
		WindowInfo->texelsoffset = 0.5;
		WindowInfo->texeltoffset = -0.5;
	}

	void Display::finalize()
	{
		if( m_context != EGL_NO_CONTEXT )
		{
			eglDestroyContext( m_display, m_context );
			m_context = EGL_NO_CONTEXT;
		}
		if( m_surface != EGL_NO_SURFACE )
		{
			eglDestroySurface( m_display, m_surface );
			m_surface = EGL_NO_SURFACE;
		}
		if( m_display != EGL_NO_DISPLAY )
		{
			eglMakeCurrent(
				m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
			eglTerminate( m_display );
			m_display = EGL_NO_DISPLAY;
		}
		if( m_hdc != 0 )
		{
			ReleaseDC( m_hwnd, m_hdc );
			m_hdc = 0;
		}
	}

	Display::Display( HWND hwnd )
		: m_hwnd( hwnd )
		, m_hdc( 0 )
		, m_display( EGL_NO_DISPLAY )
		, m_surface( EGL_NO_SURFACE )
		, m_context( EGL_NO_CONTEXT )
	{
		try
		{
			initialize();
		}
		catch( ... )
		{
			finalize();
			throw;
		}
	}

	Display::~Display()
	{
		m_shape = nullptr;
		Context::cleanup();
		finalize();
	}

	void Display::paint()
	{
		utils::Ref< graphics::Shape > shape = m_shape;
		if( shape )
		{
			Context::DrawnFrame += 1;
			shape->advance();
			shape->paint();
		}
		else
		{
			glClearColor( 1.0f, 0.3f, 0.3f, 1.0f );
			checkerror();
			glClear( GL_COLOR_BUFFER_BIT );
			checkerror();
		}
		eglSwapBuffers( m_display, m_surface );
		Context::cleanup();
	}

	void Display::setshape( Shape* nv )
	{
		m_shape = nv;
	}
}
