#ifndef GRAPHICS_TEXTURE_HPP__
#define GRAPHICS_TEXTURE_HPP__ 1

#include "resource.hpp"
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = texture )
	class Texture: public Resource
	{
	public:
		enum
		{
			MinFilter_Nearest = 0,
			MinFilter_Linear = 1,
			MinFilter_NearestMipNearest = 2,
			MinFilter_NearestMipLinear = 3,
			MinFilter_LinearMipNearest = 4,
			MinFilter_LinearMipLinear = 5,
			MinFilter_Invalid = 6,
			MagFilter_Nearest = 0,
			MagFilter_Linear = 1,
			MagFilter_Invalid = 2,
			WrapMode_Repeat = 0,
			WrapMode_Clamp = 1,
			WrapMode_Mirror = 2,
			WrapMode_Invalid = 3,
		};

	protected:
		IDirect3DTexture9* m_texture;

	private:
		int m_minfilter;
		int m_mipfilter;
		int m_magfilter;
		int m_wrapu;
		int m_wrapv;

	public:
		Texture();
		virtual ~Texture() override;
		Texture( Texture const& ) = delete;
		Texture& operator=( Texture const& ) = delete;

		bool bind( int sampler );
		R_METHOD() void setminfilter( int value );
		R_METHOD() void setmagfilter( int value );
		R_METHOD() void setwrapmode( int wu, int wv );
	};
}

#endif
