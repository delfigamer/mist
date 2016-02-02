#ifndef GRAPHICS_TEXTURE_HPP__
#define GRAPHICS_TEXTURE_HPP__ 1

#include "resource.hpp"

namespace graphics
{
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
			WrapMode_Invalid = 2
		};

	protected:
		GLuint m_texture;

	private:
		int m_minfilter;
		int m_mipfilter;
		int m_magfilter;
		int m_wraps;
		int m_wrapt;

	protected:
		virtual void doadvance( int framecount ) override;
		virtual void update() = 0;

	public:
		Texture();
		virtual ~Texture() override;
		Texture( Texture const& ) = delete;
		Texture& operator=( Texture const& ) = delete;

		void bind( int sampler );
		// static void unbind();
		void setminfilter( int value );
		void setmagfilter( int value );
		void setwrapmode( int ws, int wt );
	};

	bool graphics_texture_setminfilter( Texture* t, int value ) noexcept;
	bool graphics_texture_setmagfilter( Texture* t, int value ) noexcept;
	bool graphics_texture_setwrapmode( Texture* t, int ws, int wt ) noexcept;
}

#endif
