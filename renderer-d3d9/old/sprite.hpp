#ifndef GRAPHICS_SPRITE_HPP__
#define GRAPHICS_SPRITE_HPP__ 1

#include <utils/counterlock.hpp>
#include <atomic>
#include <mutex>

namespace graphics {
	class Sprite {
	public:
		struct Data {
			float pos_x, pos_y;
			float ext_x1, ext_y1, ext_x2, ext_y2;
			float tex1_x1, tex1_x2, tex1_y1, tex1_y2;
			float tex2_x1, tex2_x2, tex2_y1, tex2_y2;
			uint32_t color;
		};
		
	private:
		static std::atomic< int > s_order;
		
	private:
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		std::atomic< int > const m_gorder;
		std::atomic< int > const m_order;
		mutex_t m_mutex;
		Data m_data;
		
	public:
		Sprite() = delete;
		Sprite( int order );
		~Sprite();
		Sprite( Sprite const& other ) = delete;
		Sprite( Sprite&& other ) = delete;
		Sprite& operator=( Sprite const& other ) = delete;
		Sprite& operator=( Sprite&& other ) = delete;
		
		void getdata( Data* data );
		void setposition( float x, float y );
		void setextent( float x1, float y1, float x2, float y2 );
		void settexregion1( float x1, float y1, float x2, float y2 );
		void settexregion2( float x1, float y1, float x2, float y2 );
		void setcolor( float const* color );
		bool operator<( Sprite const& other ) const;
	};
	
	extern "C" {
		bool graphics_sprite_setposition( Sprite* sprite, float x, float y ) noexcept;
		bool graphics_sprite_setextent( Sprite* sprite, float x1, float y1, float x2, float y2 ) noexcept;
		bool graphics_sprite_settexregion1( Sprite* sprite, float x1, float y1, float x2, float y2 ) noexcept;
		bool graphics_sprite_settexregion2( Sprite* sprite, float x1, float y1, float x2, float y2 ) noexcept;
		bool graphics_sprite_setcolor( Sprite* sprite, float const* color ) noexcept;
	}
}

#endif
