#ifndef GRAPHICS_TEXTURE_HPP__
#define GRAPHICS_TEXTURE_HPP__ 1

#include "resource.hpp"
#include "opengl.hpp"

namespace graphics {
	class Texture: public Resource {
	public:
		enum {
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
		GLuint m_name;
		
	private:
		GLint m_minfilter;
		GLint m_magfilter;
		GLint m_wraps;
		GLint m_wrapt;
		
	protected:
		virtual void update() = 0;
		
	public:
		virtual bool query( int id, void** target ) override;
		Texture();
		Texture( Texture const& ) = delete;
		Texture( Texture&& ) = delete;
		virtual ~Texture() override;
		Texture& operator=( Texture const& ) = delete;
		Texture& operator=( Texture&& ) = delete;
		
		virtual void advance() override;
		bool istexture();
		void bind();
		static void unbind();
		void setminfilter( int value );
		void setmagfilter( int value );
		void setwrapmode( int ws, int wt );
	};
}

DEFINE_REFID( graphics::Texture, graphics::Resource, 0xefcc59c5 )

namespace graphics {
	extern "C" {
		bool graphics_texture_setminfilter( Texture* t, int value ) noexcept;
		bool graphics_texture_setmagfilter( Texture* t, int value ) noexcept;
		bool graphics_texture_setwrapmode( Texture* t, int ws, int wt ) noexcept;
	}
}

#endif
