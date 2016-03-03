#include "shapegroup.hpp"
#include <utils/cbase.hpp>
#include <utility>
#include <cstdio>

namespace graphics
{
	void ShapeGroup::doadvance()
	{
		if( !m_active.load( std::memory_order_acquire ) )
		{
			return;
		}
		lock_t lock( m_mutex );
		for( auto it: m_items )
		{
			it.second->advance();
		}
	}

	ShapeGroup::ShapeGroup()
		: m_active( true )
	{
	}

	ShapeGroup::~ShapeGroup()
	{
	}

	void ShapeGroup::paint()
	{
		if( !m_active.load( std::memory_order_acquire ) )
		{
			return;
		}
		lock_t lock( m_mutex );
		for( auto it: m_items )
		{
			it.second->paint();
		}
	}

	ShapeGroupEntry* ShapeGroup::insert( Shape* item, int order )
	{
		lock_t lock( m_mutex );
		std::pair< int, utils::Ref< Shape > > pair(
			order, utils::Ref< Shape >( item ) );
		items_t::iterator it = m_items.insert( pair );
		return new ShapeGroupEntry( this, it );
	}

	void ShapeGroup::setactive( bool active )
	{
		m_active.store( active, std::memory_order_release );
	}

	ShapeGroupEntry::ShapeGroupEntry(
		ShapeGroup* sg, ShapeGroup::items_t::iterator const& iter )
		: m_shapegroup( sg )
		, m_iter( iter )
	{
	}

	ShapeGroupEntry::~ShapeGroupEntry()
	{
		m_shapegroup->m_items.erase( m_iter );
	}
}
