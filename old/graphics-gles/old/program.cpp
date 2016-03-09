#include "program.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	bool Program::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	Program::Program() :
		m_name( 0 ) ,
		m_linked( false ) ,
		m_valid( false ) {
	}

	Program::~Program() {
		glDeleteProgram( m_name );	checkerror();
	}

	void Program::advanceentry( Program::ShaderEntry* entry, bool* changed ) {
		utils::Ref< Shader > pending = std::move( entry->pending );
		if( pending ) {
			*changed = true;
			if( entry->current ) {
				entry->current->detach( m_name );
			}
			pending->advance();
			entry->current = pending;
			pending->attach( m_name );
		} else {
			if( entry->current ) {
				entry->current->advance();
				int ccounter = entry->current->getcompilecounter();
				if( ccounter != entry->compilecounter ) {
					*changed = true;
					entry->compilecounter = ccounter;
				}
			}
		}
	}

	void Program::advance() {
		if( m_name == 0 ) {
			m_name = glCreateProgram();
			if( m_name == 0 ) {
				checkerror();
			}
		}
		bool changed = false;
		LOG( "- %#10x", uint32_t( this ) );
		advanceentry( &m_vertexshader, &changed );
		LOG( "- %#10x", uint32_t( this ) );
		advanceentry( &m_fragmentshader, &changed );
		LOG( "- %#10x", uint32_t( this ) );
		if( changed ) {
			lock_t _lock_guard( m_mutex );
		LOG( "- %#10x", uint32_t( this ) );
			glLinkProgram( m_name );	checkerror();
		LOG( "- %#10x", uint32_t( this ) );
			glGetProgramiv( m_name, GL_LINK_STATUS, &m_linked );	checkerror();
		LOG( "- %#10x", uint32_t( this ) );
			if( m_linked ) {
		LOG( "- %#10x", uint32_t( this ) );
				glValidateProgram( m_name );	checkerror();
		LOG( "- %#10x", uint32_t( this ) );
				glGetProgramiv( m_name, GL_VALIDATE_STATUS, &m_valid );	checkerror();
		LOG( "- %#10x", uint32_t( this ) );
			} else {
		LOG( "- %#10x", uint32_t( this ) );
				m_valid = false;
			}
		LOG( "- %#10x", uint32_t( this ) );
			if( !m_valid ) {
				int len;
		LOG( "- %#10x", uint32_t( this ) );
				glGetProgramiv( m_name, GL_INFO_LOG_LENGTH, &len );	checkerror();
		LOG( "- %#10x", uint32_t( this ) );
				m_log = utils::StringInstance::create( len-1, len-1, 0 );
		LOG( "- %#10x", uint32_t( this ) );
				glGetProgramInfoLog( m_name, len, 0, ( char* )m_log->m_chars );	checkerror();
		LOG( "- %#10x", uint32_t( this ) );
			}
		}
	}

	void Program::use() {
		glUseProgram( m_name ); checkerror();
	}

	bool Program::isprogram() {
		return glIsProgram( m_name );
	}

	void Program::attach( Shader* shader ) {
		switch( shader->gettype() ) {
		case GL_VERTEX_SHADER:
			m_vertexshader.pending = shader;
			break;
		case GL_FRAGMENT_SHADER:
			m_fragmentshader.pending = shader;
			break;
		}
	}

	utils::StringInstance* Program::takelog() {
		lock_t _lock_guard( m_mutex );
		if( m_log ) {
			m_log->addref();
		}
		return m_log;
	}

	bool Program::islinked() {
		lock_t _lock_guard( m_mutex );
		return m_linked;
	}

	bool Program::isvalid() {
		lock_t _lock_guard( m_mutex );
		return m_valid;
	}

	extern "C" {
	}
}
