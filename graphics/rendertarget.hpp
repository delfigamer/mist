#ifndef GRAPHICS_RENDERTARGET_HPP__
#define GRAPHICS_RENDERTARGET_HPP__ 1

#include <graphics/texture.hpp>
#include <graphics/shape.hpp>
#include <utils/ref.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = rendertarget )
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
		virtual void doadvance() override;

	public:
		RenderTarget( int format, int width, int height );
		virtual ~RenderTarget() override;
		RenderTarget( RenderTarget const& ) = delete;
		RenderTarget& operator=( RenderTarget const& ) = delete;

		R_METHOD() static RenderTarget* create(
			int format, int width, int height )
		{
			return new RenderTarget( format, width, height );
		}
		R_METHOD() void setshape( Shape* shape );
		R_METHOD() static bool isformatsupported( int format ) noexcept;
	};
}

#endif
