#ifndef GRAPHICS_SHADER_HPP__
#define GRAPHICS_SHADER_HPP__ 1

#include "resource.hpp"
#include <utils/string.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <d3dx9shader.h>

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
			ShaderStage_Vertex = 0,
			ShaderStage_Fragment = 1,
			ShaderStage_Invalid = 2,
		};

	private:
		int m_stage;
		union
		{
			IUnknown* m_shader;
			IDirect3DVertexShader9* m_vertexshader;
			IDirect3DPixelShader9* m_pixelshader;
		};
		utils::Ref< utils::DataBuffer > m_source;
		utils::String m_log;
		ID3DXConstantTable* m_constants;

	protected:
		virtual void doadvance() override;

	public:
		Shader( int format, int stage, utils::DataBuffer* source );
		virtual ~Shader() override;
		Shader( Shader const& ) = delete;
		Shader& operator=( Shader const& ) = delete;

		void getstate( bool* ready, bool* valid, char const** log );
		int getstage();
		bool bind( ID3DXConstantTable** ct );
	};

	Shader* graphics_shader_new(
		int format, int stage, utils::DataBuffer* source ) noexcept;
	bool graphics_shader_getstate( Shader* shader,
		bool* ready, bool* valid, char const** log ) noexcept;
	int graphics_shader_isformatsupported( int format ) noexcept;
}

#endif
