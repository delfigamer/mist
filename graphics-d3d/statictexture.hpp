#ifndef GRAPHICS_STATICTEXTURE_HPP__
#define GRAPHICS_STATICTEXTURE_HPP__ 1

#include "texture.hpp"
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>

namespace graphics
{
	class StaticTexture: public Texture
	{
	public:
		enum
		{
			Format_RGBA8 = 0,
			Format_RGBA16 = 1,
			Format_Invalid = 2,
		};

	private:
		int m_format;
		utils::Ref< utils::DataBuffer > m_levels[ 16 ];
		int m_width;
		int m_height;
		int m_levelcount;

	protected:
		virtual void advance() override;

	public:
		StaticTexture(
			int format, utils::DataBuffer** levels, int width, int height );
		virtual ~StaticTexture() override;
		StaticTexture( StaticTexture const& ) = delete;
		StaticTexture& operator=( StaticTexture const& ) = delete;
	};

	StaticTexture* graphics_statictexture_new(
		int format, utils::DataBuffer** levels, int width, int height ) noexcept;
	int graphics_statictexture_isformatsupported( int format ) noexcept;
}

#endif
