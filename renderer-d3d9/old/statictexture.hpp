#pragma once

#include <renderer-d3d9/texture.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = statictexture )
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
		virtual void doadvance() override;

	public:
		StaticTexture(
			int format, utils::DataBuffer** levels, int width, int height );
		virtual ~StaticTexture() override;
		StaticTexture( StaticTexture const& ) = delete;
		StaticTexture& operator=( StaticTexture const& ) = delete;

		R_METHOD() static StaticTexture* create(
			int format, utils::DataBuffer** levels, int width, int height )
		{
			return new StaticTexture( format, levels, width, height );
		}
		R_METHOD() static bool isformatsupported( int format ) noexcept;
	};
}
