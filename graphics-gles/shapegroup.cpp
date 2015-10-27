#include "shapegroup.hpp"
#include <utils/cbase.hpp>
#include <utility>
#include <cstdio>

namespace graphics {
	void ShapeGroup::doadvance( IDirect3DDevice9* device, int framecount ) {
		if( !m_active.load( std::memory_order_relaxed ) ) {
			return;
		}
		std::lock_guard< Monitor > _lock_guard( *this );
		items_t::const_iterator it = m_items.begin();
		items_t::const_iterator end = m_items.end();
		while( it != end ) {
			it->second->advance( device, framecount );
			++it;
		}
	}

	bool ShapeGroup::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
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
		std::lock_guard< Monitor > _lock_guard( *this );
		items_t::const_iterator it = m_items.begin();
		items_t::const_iterator end = m_items.end();
		while( it != end ) {
			it->second->paint( device );
			++it;
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

// 	void ShapeGroup::begin( iterator* it ) {
// 		std::lock_guard< Monitor > _lock_guard( *this );
// 		*( items_t::iterator* )it = m_items.begin();
// 	}
//
// 	void ShapeGroup::end( iterator* it ) {
// 		std::lock_guard< Monitor > _lock_guard( *this );
// 		*( items_t::iterator* )it = m_items.end();
// 	}

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

// 		bool graphics_shapegroup_begin( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept {
// 		CBASE_PROTECT(
// 			shape->begin( ( ShapeGroup::iterator* )it );
// 			return 1;
// 		)
// 		}
//
// 		bool graphics_shapegroup_end( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept {
// 		CBASE_PROTECT(
// 			shape->end( ( ShapeGroup::iterator* )it );
// 			return 1;
// 		)
// 		}
//
// 		bool graphics_shapegroup_iterator_inc( ShapeGroup_iterator* it ) noexcept {
// 		CBASE_PROTECT(
// 			++ *( ShapeGroup::iterator* )it;
// 			return 1;
// 		)
// 		}
//
// 		int graphics_shapegroup_iterator_eq( ShapeGroup_iterator* a, ShapeGroup_iterator* b ) noexcept {
// 		CBASE_PROTECT(
// 			return
// 				*( ShapeGroup::iterator* )a == *( ShapeGroup::iterator* )b ?
// 				1 : 2;
// 		)
// 		}
//
// 		bool graphics_shapegroup_iterator_deref( ShapeGroup_iterator* it, Shape** item, int* order ) noexcept {
// 		CBASE_PROTECT(
// 			ShapeGroup::iterator* sgit = ( ShapeGroup::iterator* )it;
// 			*item = ( *sgit )->second;
// 			*order = ( *sgit )->first;
// 			return 1;
// 		)
// 		}
//
// 		bool graphics_shapegroup_iterator_close( ShapeGroup_iterator* it ) noexcept {
// 		CBASE_PROTECT(
// 			( ( ShapeGroup::iterator* )it )->~iterator();
// 		)
// 		}

		bool graphics_shapegroup_setactive( ShapeGroup* shape, bool active ) noexcept {
		CBASE_PROTECT(
			shape->setactive( active );
			return 1;
		)
		}
	}
}
