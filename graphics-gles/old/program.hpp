#ifndef GRAPHICS_PROGRAM_HPP__
#define GRAPHICS_PROGRAM_HPP__ 1

#include "shader.hpp"
#include "resource.hpp"
#include "opengl.hpp"
#include <utils/counterlock.hpp>
#include <utils/string.hpp>
#include <utils/stringinstance.hpp>
#include <utils/ref.hpp>
#include <mutex>

namespace graphics {
	class Program: public Resource {
	private:
		struct ShaderEntry {
			utils::Ref< Shader > current;
			utils::Ref< Shader > pending;
			int compilecounter;
		};
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		mutex_t m_mutex;
		GLuint m_name;
		ShaderEntry m_vertexshader;
		ShaderEntry m_fragmentshader;
		int m_linked;
		int m_valid;
		utils::Ref< utils::StringInstance > m_log;
		
		void advanceentry( ShaderEntry* entry, bool* changed );
		
	public:
		virtual bool query( int id, void** target ) override;
		Program();
		virtual ~Program() override;
		Program( Program const& ) = delete;
		Program( Program&& ) = delete;
		Program& operator=( Program const& ) = delete;
		Program& operator=( Program&& ) = delete;
		
		virtual void advance() override;
		bool isprogram();
		void use();
		void attach( Shader* shader );
		utils::StringInstance* takelog();
		bool islinked();
		bool isvalid();
	};
}

DEFINE_REFID( graphics::Program, graphics::Program, 0x557062aa )

namespace graphics {
	extern "C" {
// 		Program* 
	}
}

#endif
