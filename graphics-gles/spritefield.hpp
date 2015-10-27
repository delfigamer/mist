#ifndef GRAPHICS_SPRITEFIELD_HPP__
#define GRAPHICS_SPRITEFIELD_HPP__ 1

#include "shape.hpp"
#include "sprite.hpp"
#include "pixelprogram.hpp"
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
		enum {
			Method_Replace = 0,
			Method_AlphaBlend = 1,
			Method_Add = 2,
			Method_AddSaturated = 3,
			Method_Invalid = 4,
		};
		
	public:
		struct Vertex {
			float pos_x, pos_y, pos_z;
			float tex1_x, tex1_y;
			float tex2_x, tex2_y;
			uint32_t color;
		};
		
	private:
		struct SpriteComparer {
			bool operator()( Sprite* lhs, Sprite* rhs ) const;
		};
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		mutex_t m_mutex;
		utils::Ref< PixelProgram > m_pixelprogram;
		int m_buffersize;
		IDirect3DVertexBuffer9* m_vertexbuffer;
		IDirect3DIndexBuffer9* m_indexbuffer;
		IDirect3DVertexDeclaration9* m_vertexdeclaration;
		std::multiset< Sprite*, SpriteComparer > m_sprites;
		std::stack< Sprite* > m_newsprites;
		mutex_t m_modemutex;
		int m_blendsf;
		int m_blenddf;
		int m_blendop;
		utils::Ref< Texture > m_textures[ 8 ];
		D3DMATRIX m_matrix;
		
	protected:
		virtual void doadvance( IDirect3DDevice9* device, int framecount ) override;
		
	public:
		virtual bool query( int id, void** target ) override;
		SpriteField();
		SpriteField( SpriteField const& ) = delete;
		SpriteField( SpriteField&& ) = delete;
		virtual ~SpriteField();
		SpriteField& operator=( SpriteField const& ) = delete;
		SpriteField& operator=( SpriteField&& ) = delete;
		
		virtual void paint( IDirect3DDevice9* device ) override;
		void setpixelprogram( PixelProgram* pixelprogram );
		Sprite* newsprite( int order );
		void removesprite( Sprite* sprite );
		void setblendmethod( int method );
		void settexture( int stage, Texture* texture );
		void setmatrix( float const* data );
	};
}

DEFINE_REFID( graphics::SpriteField, graphics::Shape, 0xe166eadf )

namespace graphics {
	extern "C" {
		SpriteField* graphics_spritefield_new() noexcept;
		bool graphics_spritefield_setpixelprogram( SpriteField* sf, PixelProgram* pixelprogram ) noexcept;
		Sprite* graphics_spritefield_newsprite( SpriteField* sf, int order ) noexcept;
		bool graphics_spritefield_removesprite( SpriteField* sf, Sprite* sprite ) noexcept;
		bool graphics_spritefield_setblendmethod( SpriteField* sf, int method ) noexcept;
		bool graphics_spritefield_settexture( SpriteField* sf, int stage, Texture* texture ) noexcept;
		bool graphics_spritefield_setmatrix( SpriteField* sf, float const* data ) noexcept;
	}
}

#endif
