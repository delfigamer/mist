#ifndef GRAPHICS_SPRITE_HPP__
#define GRAPHICS_SPRITE_HPP__ 1

#include "opengl.hpp"
#include <utils/counterlock.hpp>
#include <atomic>
#include <mutex>

namespace graphics {
	class Sprite {
	public:
		struct Data {
			GLfloat pos_x, pos_y;
			GLfloat ext_x1, ext_y1, ext_x2, ext_y2;
			GLfloat tex_x1, tex_x2, tex_y1, tex_y2;
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
		void setposition( GLfloat x, GLfloat y );
		void setextent( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
		void settexregion( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
		bool operator<( Sprite const& other ) const;
	};
	
	extern "C" {
		bool graphics_sprite_setposition( Sprite* sprite, GLfloat x, GLfloat y ) noexcept;
		bool graphics_sprite_setextent( Sprite* sprite, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) noexcept;
		bool graphics_sprite_settexregion( Sprite* sprite, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) noexcept;
	}
}

#endif
