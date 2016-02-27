#ifndef GRAPHICS_RENDERTARGET_HPP__
#define GRAPHICS_RENDERTARGET_HPP__ 1

#include "texture.hpp"
#include "shape.hpp"
#include <utils/ref.hpp>

namespace graphics
{
	class RenderTarget: public Texture
	{
	public:
		enum
		{
			RTFormat_RGBA8 = 0,
			RTFormat_RGBA8D24S8 = 1,
			RTFormat_Invalid = 2,
		};

	private:
		IDirect3DSurface9* m_dssurface;
		int m_texformat;
		int m_dsformat;
		int m_width;
		int m_height;
		utils::Ref< Shape > m_shape;

	protected:
		virtual void advance() override;

	public:
		RenderTarget( int format, int width, int height );
		virtual ~RenderTarget() override;
		RenderTarget( RenderTarget const& ) = delete;
		RenderTarget& operator=( RenderTarget const& ) = delete;

		void setshape( Shape* shape );
	};

	RenderTarget* graphics_rendertarget_new(
		int format, int width, int height ) noexcept;
	bool graphics_rendertarget_setshape(
		RenderTarget* rt, Shape* shape ) noexcept;
}

#endif
