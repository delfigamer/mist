#include <graphics/program.hpp>
#include <graphics/common.hpp>
#include <utils/console.hpp>
#include <stdexcept>
#include <cstring>

namespace graphics
{
	void Program::doadvance()
	{
		m_vertexshader->advance();
		m_fragmentshader->advance();
	}

	Program::Program( Shader* vs, Shader* fs )
	{
		if( !vs || vs->getstage() != Shader::ShaderStage_Vertex )
		{
			throw std::runtime_error( "vertex shader expected" );
		}
		if( !fs || fs->getstage() != Shader::ShaderStage_Fragment )
		{
			throw std::runtime_error( "fragment shader expected" );
		}
		m_vertexshader = vs;
		m_fragmentshader = fs;
	}

	Program::~Program()
	{
	}

	bool Program::bind( int* worldmatrixindex )
	{
		ID3DXConstantTable* ct;
		if( !m_vertexshader->bind( &ct ) )
		{
			return false;
		}
		*worldmatrixindex = 0;
		if( !m_fragmentshader->bind( &ct ) )
		{
			return false;
		}
		return true;
	}
}
