#ifndef GRAPHICS_SPRITEFIELD_HPP__
#define GRAPHICS_SPRITEFIELD_HPP__ 1

#include "shape.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include <utils/counterlock.hpp>
#include <utils/ref.hpp>
#include <stack>
#include <vector>
#include <set>
#include <mutex>

namespace graphics {
	class SpriteField: public Shape {
	public:
		struct Vertex {
			GLfloat pos_x, pos_y;
			GLfloat tex_x, tex_y;
			GLfloat reserved[ 4 ];
		};
		
	private:
		struct SpriteComparer {
			bool operator()( Sprite* lhs, Sprite* rhs ) const;
		};
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		mutex_t m_mutex;
		utils::Ref< Texture > m_texture;
		std::vector< Vertex > m_vertices;
		std::vector< GLshort > m_indices;
		std::multiset< Sprite*, SpriteComparer > m_sprites;
		std::stack< Sprite* > m_newsprites;
		
	public:
		virtual bool query( int id, void** target ) override;
		SpriteField();
		SpriteField( SpriteField const& ) = delete;
		SpriteField( SpriteField&& ) = delete;
		virtual ~SpriteField();
		SpriteField& operator=( SpriteField const& ) = delete;
		SpriteField& operator=( SpriteField&& ) = delete;
		
		virtual void paint() override;
		virtual void advance() override;
		Texture* gettexture() const;
		void settexture( Texture* texture );
		Sprite* newsprite( int order );
		void removesprite( Sprite* sprite );
	};
}

DEFINE_REFID( graphics::SpriteField, graphics::Shape, 0xe166eadf )

namespace graphics {
	extern "C" {
		SpriteField* graphics_spritefield_new() noexcept;
		Texture* graphics_spritefield_gettexture( SpriteField* sf ) noexcept;
		bool graphics_spritefield_settexture( SpriteField* sf, Texture* texture ) noexcept;
		Sprite* graphics_spritefield_newsprite( SpriteField* sf, int order ) noexcept;
		bool graphics_spritefield_removesprite( SpriteField* sf, Sprite* sprite ) noexcept;
	}
}

#endif
