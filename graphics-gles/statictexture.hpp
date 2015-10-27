#ifndef GRAPHICS_STATICTEXTURE_HPP__
#define GRAPHICS_STATICTEXTURE_HPP__ 1

#include "texture.hpp"
#include <utils/stringinstance.hpp>
#include <utils/counterlock.hpp>
#include <mutex>

namespace graphics {
	class StaticTexture: public Texture {
	private:
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		mutex_t m_mutex;
		utils::Ref< utils::StringInstance > m_data;
		int m_width;
		int m_height;
		
	protected:
		virtual void update( IDirect3DDevice9* device ) override;
		
	public:
		virtual bool query( int id, void** target ) override;
		StaticTexture();
		StaticTexture( StaticTexture const& ) = delete;
		StaticTexture( StaticTexture&& ) = delete;
		virtual ~StaticTexture();
		StaticTexture& operator=( StaticTexture const& ) = delete;
		StaticTexture& operator=( StaticTexture&& ) = delete;
		
		void assign( utils::StringInstance* data, int width, int height );
	};
}

DEFINE_REFID( graphics::StaticTexture, graphics::Texture, 0xfc1d0514 )

namespace graphics {
	extern "C" {
		StaticTexture* graphics_statictexture_new() noexcept;
		bool graphics_statictexture_assign( StaticTexture* st, utils::StringInstance* data, int width, int height ) noexcept;
	}
}

#endif
