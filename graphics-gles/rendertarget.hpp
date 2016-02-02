#ifndef GRAPHICS_RENDERTARGET_HPP__
#define GRAPHICS_RENDERTARGET_HPP__ 1

#include "shape.hpp"
#include "texture.hpp"
#include <utils/ref.hpp>
#include <utils/flaglock.hpp>
#include <mutex>

namespace graphics
{
	class RenderTarget: public Texture
	{
	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		GLuint m_framebuffer;
		int m_format;
		int m_width;
		int m_height;
		utils::Ref< Shape > m_shape;
		int m_clearflags;
		uint32_t m_clearcolor;
		float m_cleardepth;
		int m_clearstencil;

	protected:
		virtual void update() override;

	public:
		RenderTarget() = delete;
		RenderTarget( int format, int width, int height );
		virtual ~RenderTarget() override;
		RenderTarget( RenderTarget const& ) = delete;
		RenderTarget& operator=( RenderTarget const& ) = delete;

		void setdepthstenciluse( bool use );
		void setshape( Shape* shape );
		void setclearcolor( bool flag, float const* value );
		void setcleardepth( bool flag, float value );
		void setclearstencil( bool flag, int value );
	};

	RenderTarget* graphics_rendertarget_new(
		int format, int width, int height ) noexcept;
	bool graphics_rendertarget_setdepthstenciluse(
		RenderTarget* rt, bool use ) noexcept;
	bool graphics_rendertarget_setshape(
		RenderTarget* rt, Shape* shape ) noexcept;
	bool graphics_rendertarget_setclearcolor(
		RenderTarget* rt, bool flag, float const* value ) noexcept;
	bool graphics_rendertarget_setcleardepth(
		RenderTarget* rt, bool flag, float value ) noexcept;
	bool graphics_rendertarget_setclearstencil(
		RenderTarget* rt, bool flag, int value ) noexcept;
}

#endif
