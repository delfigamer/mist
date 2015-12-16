#ifndef GRAPHICS_STATICTEXTURE_HPP__
#define GRAPHICS_STATICTEXTURE_HPP__ 1

#include "texture.hpp"
#include <utils/databuffer.hpp>
#include <utils/flaglock.hpp>
#include <mutex>

namespace graphics
{
	class StaticTexture: public Texture
	{
	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		int m_format;
		utils::Ref< utils::DataBuffer > m_data;
		int m_width;
		int m_height;

	protected:
		virtual void update( IDirect3DDevice9* device ) override;

	public:
		StaticTexture();
		virtual ~StaticTexture();
		StaticTexture( StaticTexture const& ) = delete;
		StaticTexture( StaticTexture&& ) = delete;
		StaticTexture& operator=( StaticTexture const& ) = delete;
		StaticTexture& operator=( StaticTexture&& ) = delete;

		void assign( int format, utils::DataBuffer* data, int width, int height );
	};

		StaticTexture* graphics_statictexture_new() noexcept;
		bool graphics_statictexture_assign(
			StaticTexture* st,
			int format, utils::DataBuffer* data, int width, int height ) noexcept;
}

#endif
