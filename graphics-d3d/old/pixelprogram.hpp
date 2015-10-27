#ifndef GRAPHICS_PIXELPROGRAM_HPP__
#define GRAPHICS_PIXELPROGRAM_HPP__ 1

#include "texture.hpp"
#include "resource.hpp"
#include <utils/ref.hpp>
#include <utils/counterlock.hpp>
#include <mutex>

namespace graphics {
	class PixelProgram: public Resource {
	public:
		enum {
			Op_Disable = 0,
			Op_SelectArg1 = 1,
			Op_SelectArg2 = 2,
			Op_Modulate = 3,
			Op_Modulate2x = 4,
			Op_Modulate4x = 5,
			Op_Add = 6,
			Op_AddSigned = 7,
			Op_AddSigned2x = 8,
			Op_Subtract = 9,
			Op_AddSmooth = 10,
			Op_BlendDiffuseAlpha = 11,
			Op_BlendTextureAlpha = 12,
			Op_BlendCurrentAlpha = 13,
			Op_DotProduct = 14,
			Op_Invalid = 15,
			Arg_Constant = 0,
			Arg_Current = 1,
			Arg_Diffuse = 2,
			Arg_Texture = 3,
			Arg_Invalid = 4,
			TexCoord_Screen = 0,
			TexCoord_Tex1 = 1,
			TexCoord_Tex2 = 2,
			TexCoord_Invalid = 3,
			Matrix_Identity = 0,
			Matrix_Mat1 = 1,
			Matrix_Mat2 = 2,
			Matrix_Invalid = 3,
		};

	private:
		struct Stage {
			utils::Ref< Texture > texture;
			uint32_t constant;
			int colorop;
			int colorarg1;
			int colorarg2;
			int alphaop;
			int alphaarg1;
			int alphaarg2;
			int texcoord;
			D3DMATRIX* matrix;
		};
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		Stage m_stages[ 8 ];
		D3DMATRIX m_matrices[ 2 ];
		IDirect3DVertexShader9* m_vertexshader;
		IDirect3DPixelShader9* m_pixelshader;

	protected:
		virtual void doadvance( IDirect3DDevice9* device, int framecount ) override;

	public:
		PixelProgram();
		PixelProgram( PixelProgram const& ) = delete;
		PixelProgram( PixelProgram&& ) = delete;
		virtual ~PixelProgram() override;
		PixelProgram& operator=( PixelProgram const& ) = delete;
		PixelProgram& operator=( PixelProgram&& ) = delete;

		void bind( IDirect3DDevice9* device, utils::Ref< Texture > const* textures );
// 		static void unbind();
		void setstagetexture( int stage, Texture* texture );
		void setstageconstant( int stage, float const* constant );
		void setstagecolor( int stage, int op, int arg1, int arg2 );
		void setstagealpha( int stage, int op, int arg1, int arg2 );
		void setstagetexcoord( int stage, int index );
		void setstagematrix( int stage, int index );
		void setmatrix( int index, float const* data );
	};

	extern "C" {
		PixelProgram* graphics_pixelprogram_new() noexcept;
		bool graphics_pixelprogram_setstagetexture( PixelProgram* pp, int stage, Texture* texture ) noexcept;
		bool graphics_pixelprogram_setstageconstant( PixelProgram* pp, int stage, float const* constant ) noexcept;
		bool graphics_pixelprogram_setstagecolor( PixelProgram* pp, int stage, int op, int arg1, int arg2 ) noexcept;
		bool graphics_pixelprogram_setstagealpha( PixelProgram* pp, int stage, int op, int arg1, int arg2 ) noexcept;
		bool graphics_pixelprogram_setstagetexcoord( PixelProgram* pp, int stage, int index ) noexcept;
		bool graphics_pixelprogram_setstagematrix( PixelProgram* pp, int stage, int index ) noexcept;
		bool graphics_pixelprogram_setmatrix( PixelProgram* pp, int index, float const* data ) noexcept;
	}
}

#endif
