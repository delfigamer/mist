#ifndef GRAPHICS_SHADER_HPP__
#define GRAPHICS_SHADER_HPP__ 1

#include "texture.hpp"
#include "resource.hpp"
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <utils/counterlock.hpp>
#include <d3dx9shader.h>
#include <mutex>

namespace graphics
{
	class Shader: public Resource
	{
	private:
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		utils::Ref< Texture > m_textures[ 8 ];
		int m_samplerindex[ 8 ];
		IDirect3DVertexShader9* m_vertexshader;
		IDirect3DPixelShader9* m_pixelshader;
		ID3DXBuffer* m_vertexshaderdata;
		ID3DXConstantTable* m_vertexshaderconstants;
		ID3DXBuffer* m_pixelshaderdata;
		ID3DXConstantTable* m_pixelshaderconstants;

	protected:
		virtual void doadvance( IDirect3DDevice9* device, int framecount ) override;

	public:
		Shader();
		virtual ~Shader() override;
		Shader( Shader const& ) = delete;
		Shader( Shader&& ) = delete;
		Shader& operator=( Shader const& ) = delete;
		Shader& operator=( Shader&& ) = delete;

		void bind( IDirect3DDevice9* device, utils::Ref< Texture > const* textures );
// 		static void unbind();
		void settexture( int stage, Texture* texture );
		void setshadersources( char const* vert, char const* frag, char const* texnames );
	};

	extern "C"
	{
		Shader* graphics_shader_new() noexcept;
		bool graphics_shader_settexture( Shader* sh, int stage, Texture* texture ) noexcept;
		bool graphics_shader_setshadersources( Shader* sh, char const* vert, char const* frag, char const* texnames ) noexcept;
	}
}

#endif
