#ifndef GRAPHICS_PROGRAM_HPP__
#define GRAPHICS_PROGRAM_HPP__ 1

#include "shader.hpp"
#include <utils/ref.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = program )
	class Program: public Resource
	{
	private:
		utils::Ref< Shader > m_vertexshader;
		utils::Ref< Shader > m_fragmentshader;

	protected:
		virtual void doadvance() override;

	public:
		Program( Shader* vs, Shader* fs );
		virtual ~Program() override;
		Program( Program const& ) = delete;
		Program& operator=( Program const& ) = delete;

		bool bind( int* worldmatrixindex );

		R_METHOD() static Program* create(
			Shader* vs, Shader* fs )
		{
			return new Program( vs, fs );
		}
	};
}

#endif
