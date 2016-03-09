#include "rendertarget.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics
{
	void RenderTarget::update()
	{
		if( !m_texture )
		{
			glGenTextures( 1, &m_texture );
			checkerror();
			glBindTexture( GL_TEXTURE_2D, m_texture );
			checkerror();
			glTexImage2D(
				GL_TEXTURE_2D, 0,
				GL_RGBA,
				m_width, m_height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE,
				0 );
			checkerror();
		}
		/*if( m_dsuse )
		{
			if( !m_dssurface )
			{
				checkerror( device->CreateDepthStencilSurface(
					m_width, m_height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE,
					0, true, &m_dssurface, 0 ) );
			}
		}
		else
		{
			if( m_dssurface )
			{
				RELEASE( m_dssurface );
			}
		}*/
		Shape* shape = m_shape;
		if( shape )
		{
			shape->advance( m_lastframe );
			GLuint oldfb;
			glGetIntegerv( GL_FRAMEBUFFER_BINDING, ( GLint* )&oldfb );
			checkerror();
			if( m_framebuffer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, m_framebuffer );
				checkerror();
			}
			else
			{
				glGenFramebuffers( 1, &m_framebuffer );
				checkerror();
				glBindFramebuffer( GL_FRAMEBUFFER, m_framebuffer );
				checkerror();
				glFramebufferTexture2D(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_2D,
					m_texture, 0 );
				checkerror();
			}
			try
			{
				/*if( m_clearflags )
				{
					checkerror( device->Clear(
						0, 0, m_clearflags,
						m_clearcolor, m_cleardepth, m_clearstencil ) );
				}*/
				shape->paint();
			}
			catch( ... )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, oldfb );
				throw;
			}
			glBindFramebuffer( GL_FRAMEBUFFER, oldfb );
			checkerror();
		}
	}

	RenderTarget::RenderTarget( int format, int width, int height )
		: m_framebuffer( 0 )
		, m_format( format )
		, m_width( width )
		, m_height( height )
		, m_clearflags( 0 )
	{
	}

	RenderTarget::~RenderTarget()
	{
		glDeleteFramebuffers( 1, &m_framebuffer );
	}

	void RenderTarget::setdepthstenciluse( bool use )
	{
	}

	void RenderTarget::setshape( Shape* shape )
	{
		m_shape = shape;
	}

	void RenderTarget::setclearcolor( bool flag, float const* value )
	{
	}

	void RenderTarget::setcleardepth( bool flag, float value )
	{
	}

	void RenderTarget::setclearstencil( bool flag, int value )
	{
	}

	RenderTarget* graphics_rendertarget_new(
		int format, int width, int height ) noexcept
	{
	CBASE_PROTECT(
		return new RenderTarget( format, width, height );
	)
	}

	bool graphics_rendertarget_setdepthstenciluse(
		RenderTarget* rt, bool use ) noexcept
	{
	CBASE_PROTECT(
		rt->setdepthstenciluse( use );
		return 1;
	)
	}

	bool graphics_rendertarget_setshape(
		RenderTarget* rt, Shape* shape ) noexcept
	{
	CBASE_PROTECT(
		rt->setshape( shape );
		return 1;
	)
	}

	bool graphics_rendertarget_setclearcolor(
		RenderTarget* rt, bool flag, float const* value ) noexcept
	{
	CBASE_PROTECT(
		rt->setclearcolor( flag, value );
		return 1;
	)
	}

	bool graphics_rendertarget_setcleardepth(
		RenderTarget* rt, bool flag, float value ) noexcept
	{
	CBASE_PROTECT(
		rt->setcleardepth( flag, value );
		return 1;
	)
	}

	bool graphics_rendertarget_setclearstencil(
		RenderTarget* rt, bool flag, int value ) noexcept
	{
	CBASE_PROTECT(
		rt->setclearstencil( flag, value );
		return 1;
	)
	}
}
