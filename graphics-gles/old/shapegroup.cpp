#include "shapegroup.hpp"
#include "texture.hpp"
#include <utils/cbase.hpp>
#include <utility>
#include <cstdio>

namespace graphics {
	static char const VertexShaderSource[] = "#version 110\n\
\n\
attribute vec4 pointpos;\n\
attribute vec4 texpos;\n\
varying vec4 texcoord;\n\
// uniform mat4 projection;\n\
// uniform mat4 modelview;\n\
\n\
void main() {\n\
	gl_Position = pointpos;\n\
	texcoord = texpos;\n\
}";
	static char const FragmentShaderSource[] = "#version 110\n\
varying vec4 texcoord;\n\
\n\
void main() {\n\
	gl_FragColor = texcoord;\n\
}";

	bool ShapeGroup::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	ShapeGroup::ShapeGroup() {
	}

	ShapeGroup::~ShapeGroup() {
	}

	void ShapeGroup::paint() {
		std::lock_guard< Monitor > _lock_guard( *this );
		if( m_program->isvalid() ) {
			m_program->use();
		}
		items_t::const_iterator it = m_items.begin();
		items_t::const_iterator end = m_items.end();
		while( it != end ) {
			it->second->paint();
			++it;
		}
	}

	void ShapeGroup::advance() {
		std::lock_guard< Monitor > _lock_guard( *this );
		items_t::const_iterator it = m_items.begin();
		items_t::const_iterator end = m_items.end();
		while( it != end ) {
			it->second->advance();
			++it;
		}
		if( !m_program || !m_program->isprogram() ) {
			utils::Ref< Shader > vsh = utils::Ref< Shader >::create( GL_VERTEX_SHADER );
			utils::Ref< Shader > fsh = utils::Ref< Shader >::create( GL_FRAGMENT_SHADER );
			vsh->setcode( VertexShaderSource );
			fsh->setcode( FragmentShaderSource );
			m_program = utils::Ref< Program >::create();
			m_program->attach( vsh );
			m_program->attach( fsh );
		LOG( "-" );
			m_program->advance();
		LOG( "-" );
			utils::Ref< utils::StringInstance > vlog( vsh->takelog(), 0 );
			LOG( "vertex shader: %s", vlog ? ( char const* )vlog->m_chars : "" );
			utils::Ref< utils::StringInstance > flog( fsh->takelog(), 0 );
			LOG( "fragment shader: %s", flog ? ( char const* )flog->m_chars : "" );
			utils::Ref< utils::StringInstance > plog( m_program->takelog(), 0 );
			LOG( "program: %s", plog ? ( char const* )plog->m_chars : "" );
		}
	}

	void ShapeGroup::insert( Shape* item, int order, iterator* it ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		std::pair< int, utils::Ref< Shape > > pair( order, utils::Ref< Shape >( item ) );
		*( items_t::iterator* )it = m_items.insert( pair );
	}

	void ShapeGroup::remove( iterator* it ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		*( items_t::iterator* )it = m_items.erase( *it );
	}

	void ShapeGroup::begin( iterator* it ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		*( items_t::iterator* )it = m_items.begin();
	}

	void ShapeGroup::end( iterator* it ) {
		std::lock_guard< Monitor > _lock_guard( *this );
		*( items_t::iterator* )it = m_items.end();
	}

	void ShapeGroup::setprogram( Program* program ) {
		m_program = program;
	}

	extern "C" {
		ShapeGroup* graphics_shapegroup_new() noexcept {
		CBASE_PROTECT(
			return new ShapeGroup();
		)
		}

		bool graphics_shapegroup_insert( ShapeGroup* shape, Shape* item, int order, ShapeGroup_iterator* it ) noexcept {
		CBASE_PROTECT(
			shape->insert( item, order, ( ShapeGroup::iterator* )it );
			return 1;
		)
		}

		bool graphics_shapegroup_remove( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept {
		CBASE_PROTECT(
			shape->remove( ( ShapeGroup::iterator* )it );
			return 1;
		)
		}

		bool graphics_shapegroup_begin( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept {
		CBASE_PROTECT(
			shape->begin( ( ShapeGroup::iterator* )it );
			return 1;
		)
		}

		bool graphics_shapegroup_end( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept {
		CBASE_PROTECT(
			shape->end( ( ShapeGroup::iterator* )it );
			return 1;
		)
		}

		bool graphics_shapegroup_iterator_inc( ShapeGroup_iterator* it ) noexcept {
		CBASE_PROTECT(
			++ *( ShapeGroup::iterator* )it;
			return 1;
		)
		}

		int graphics_shapegroup_iterator_eq( ShapeGroup_iterator* a, ShapeGroup_iterator* b ) noexcept {
		CBASE_PROTECT(
			return
				*( ShapeGroup::iterator* )a == *( ShapeGroup::iterator* )b ?
				1 : 2;
		)
		}

		bool graphics_shapegroup_iterator_deref( ShapeGroup_iterator* it, Shape** item, int* order ) noexcept {
		CBASE_PROTECT(
			ShapeGroup::iterator* sgit = ( ShapeGroup::iterator* )it;
			*item = ( *sgit )->second;
			*order = ( *sgit )->first;
			return 1;
		)
		}

		bool graphics_shapegroup_iterator_close( ShapeGroup_iterator* it ) noexcept {
		CBASE_PROTECT(
			( ( ShapeGroup::iterator* )it )->~iterator();
		)
		}

		bool graphics_shapegroup_setprogram( ShapeGroup* shape, Program* program ) noexcept {
		CBASE_PROTECT(
			shape->setprogram( program );
			return 1;
		)
		}
	}
}
