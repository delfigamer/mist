#include "shapegroup.hpp"
#include <utils/cbase.hpp>
#include <utility>
#include <cstdio>

namespace graphics {
	void ShapeGroup::doadvance( IDirect3DDevice9* device, int framecount ) {
		if( !m_active.load( std::memory_order_relaxed ) ) {
			return;
		}
		lock_t lock( m_mutex );
		items_t::const_iterator it = m_items.begin();
		items_t::const_iterator end = m_items.end();
		while( it != end ) {
			it->second->advance( device, framecount );
			++it;
		}
	}

	ShapeGroup::ShapeGroup() :
		m_active( true ) {
	}

	ShapeGroup::~ShapeGroup() {
	}

	void ShapeGroup::paint( IDirect3DDevice9* device ) {
		if( !m_active.load( std::memory_order_relaxed ) ) {
			return;
		}
		lock_t lock( m_mutex );
		items_t::const_iterator it = m_items.begin();
		items_t::const_iterator end = m_items.end();
		while( it != end ) {
			it->second->paint( device );
			++it;
		}
	}

	void ShapeGroup::insert( Shape* item, int order, iterator* it ) {
		lock_t lock( m_mutex );
		std::pair< int, utils::Ref< Shape > > pair( order, utils::Ref< Shape >( item ) );
		*( items_t::iterator* )it = m_items.insert( pair );
	}

	void ShapeGroup::remove( iterator* it ) {
		lock_t lock( m_mutex );
		*( items_t::iterator* )it = m_items.erase( *it );
	}

	void ShapeGroup::setactive( bool active ) {
		m_active.store( active, std::memory_order_relaxed );
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

		bool graphics_shapegroup_setactive( ShapeGroup* shape, bool active ) noexcept {
		CBASE_PROTECT(
			shape->setactive( active );
			return 1;
		)
		}
	}
}
