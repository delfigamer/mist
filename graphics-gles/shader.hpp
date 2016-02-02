#ifndef GRAPHICS_SHADER_HPP__
#define GRAPHICS_SHADER_HPP__ 1

#include "texture.hpp"
#include "resource.hpp"
#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <utils/flaglock.hpp>
#include <mutex>

namespace graphics
{
	class Shader: public Resource
	{
	public:
		enum
		{
			ShaderFormat_GLSL_100 = 0,
			ShaderFormat_HLSL_3_0_Source = 1,
			ShaderFormat_HLSL_3_0_Bytecode = 2,
			ShaderFormat_Invalid = 3,
		};

	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		utils::Ref< Texture > m_textures[ 8 ];
		GLint m_samplerindex[ 8 ];
		GLuint m_vertexshader;
		GLuint m_pixelshader;
		GLuint m_program;
		bool m_validvs;
		bool m_validps;
		bool m_validprogram;
		int m_wvmindex;
		utils::String m_vertexshaderdata;
		utils::String m_pixelshaderdata;
		utils::String m_samplerlist;

	protected:
		virtual void doadvance( int framecount ) override;

	public:
		Shader();
		virtual ~Shader() override;
		Shader( Shader const& ) = delete;
		Shader( Shader&& ) = delete;
		Shader& operator=( Shader const& ) = delete;
		Shader& operator=( Shader&& ) = delete;

		bool bind( utils::Ref< Texture > const* textures, int* wvmindex );
		// static void unbind();
		void settexture( int stage, Texture* texture );
		void setshadersources(
			int format, char const* vert, char const* frag, char const* texnames );
	};

	Shader* graphics_shader_new() noexcept;
	bool graphics_shader_settexture(
		Shader* sh, int stage, Texture* texture ) noexcept;
	bool graphics_shader_setshadersources(
		Shader* sh,
		int format,
		char const* vert, char const* frag, char const* texnames ) noexcept;
}

#endif
