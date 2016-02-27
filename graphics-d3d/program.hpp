#ifndef GRAPHICS_PROGRAM_HPP__
#define GRAPHICS_PROGRAM_HPP__ 1

#include "shader.hpp"
#include <utils/ref.hpp>

namespace graphics
{
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
	};

	Program* graphics_program_new( Shader* vs, Shader* fs ) noexcept;
}

#endif
