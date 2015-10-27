#ifndef GRAPHICS_SHADER_HPP__
#define GRAPHICS_SHADER_HPP__ 1

#include "resource.hpp"
#include "opengl.hpp"
#include <utils/counterlock.hpp>
#include <utils/string.hpp>
#include <utils/stringinstance.hpp>
#include <mutex>

namespace graphics {
	class Shader: public Resource {
	private:
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		mutex_t m_mutex;
		GLuint m_name;
		GLenum m_type;
		int m_compiled;
		int m_compilecounter;
		utils::String m_code;
		utils::Ref< utils::StringInstance > m_log;
		
	public:
		virtual bool query( int id, void** target ) override;
		Shader() = delete;
		Shader( GLenum type );
		virtual ~Shader() override;
		Shader( Shader const& ) = delete;
		Shader( Shader&& ) = delete;
		Shader& operator=( Shader const& ) = delete;
		Shader& operator=( Shader&& ) = delete;
		
		virtual void advance() override;
		bool isshader();
		GLenum gettype();
		void attach( GLuint program );
		void detach( GLuint program );
		void setcode( char const* code );
		utils::StringInstance* takelog();
		bool iscompiled();
		int getcompilecounter();
	};
}

DEFINE_REFID( graphics::Shader, graphics::Shader, 0x84e678c2 )

namespace graphics {
	extern "C" {
		Shader* graphics_shader_new( GLenum type ) noexcept;
		bool graphics_shader_setcode( Shader* shader, char const* code ) noexcept;
		utils::StringInstance* graphics_shader_takelog( Shader* shader ) noexcept;
		int graphics_shader_getcompilecounter( Shader* shader ) noexcept;
	}
}

#endif
