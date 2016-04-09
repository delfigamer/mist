#ifndef GRAPHICS_SHADER_HPP__
#define GRAPHICS_SHADER_HPP__ 1

#include <graphics/resource.hpp>
#include <utils/string.hpp>
#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <d3dx9shader.h>

namespace graphics
{
	R_CLASS( name = shader )
	class Shader: public Resource
	{
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

		int getstage();
		bool bind( ID3DXConstantTable** ct );

		R_METHOD() static Shader* create(
			int format, int stage, utils::DataBuffer* source )
		{
			return new Shader( format, stage, source );
		}
		R_METHOD() void getstate( bool* ready, bool* valid, char const** log );
		R_METHOD() static bool isformatsupported( int format ) noexcept;
	};
}

#endif
