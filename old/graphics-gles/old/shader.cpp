#include "shader.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	bool Shader::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}
	
	Shader::Shader( GLuint type ) :
		m_name( 0 ) ,
		m_type( type ) ,
		m_compiled( false ) {
	}
	
	Shader::~Shader() {
		glDeleteShader( m_name );	checkerror();
	}
	
	void Shader::advance() {
		if( m_name == 0 ) {
			m_name = glCreateShader( m_type );
			if( m_name == 0 ) {
				checkerror();
			}
		}
		utils::String code = std::move( m_code );
		if( code ) {
			lock_t _lock_guard( m_mutex );
			char const* source = code.chars();
			glShaderSource( m_name, 1, ( char const* const* )&source, 0 );	checkerror();
			glCompileShader( m_name );	checkerror();
			glGetShaderiv( m_name, GL_COMPILE_STATUS, &m_compiled );	checkerror();
			if( !m_compiled ) {
				int len;
				glGetShaderiv( m_name, GL_INFO_LOG_LENGTH, &len );	checkerror();
				m_log = utils::StringInstance::create( len-1, len-1, 0 );
				glGetShaderInfoLog( m_name, len, 0, ( char* )m_log->m_chars );	checkerror();
			}
			m_compilecounter += 1;
		}
	}
	
	bool Shader::isshader() {
		return glIsShader( m_name );
	}
	
	GLenum Shader::gettype() {
		return m_type;
	}
	
	void Shader::attach( GLuint program ) {
		glAttachShader( program, m_name );
	};
	
	void Shader::detach( GLuint program ) {
		glDetachShader( program, m_name );
	}
	
	void Shader::setcode( char const* code ) {
		m_code = code;
	}
	
	utils::StringInstance* Shader::takelog() {
		lock_t _lock_guard( m_mutex );
		if( m_log ) {
			m_log->addref();
		}
		return m_log;
	}
	
	bool Shader::iscompiled() {
		lock_t _lock_guard( m_mutex );
		return m_compiled;
	}
	
	int Shader::getcompilecounter() {
		lock_t _lock_guard( m_mutex );
		return m_compilecounter;
	}
	
	extern "C" {
		Shader* graphics_shader_new( GLenum type ) noexcept {
		CBASE_PROTECT(
			return new Shader( type );
		)
		}
		
		bool graphics_shader_setcode( Shader* shader, char const* code ) noexcept {
		CBASE_PROTECT(
			shader->setcode( code );
			return 1;
		)
		}
		
		utils::StringInstance* graphics_shader_takelog( Shader* shader ) noexcept {
		CBASE_PROTECT(
			return shader->takelog();
		)
		}
		
		int graphics_shader_getcompilecounter( Shader* shader ) noexcept {
		CBASE_PROTECT(
			return shader->getcompilecounter();
		)
		}
	}
}
