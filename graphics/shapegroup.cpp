#include <graphics/shapegroup.hpp>
#include <utils/cbase.hpp>

namespace graphics
{
	void ShapeGroup::doadvance()
	{
		if( !m_active.load( std::memory_order_relaxed ) )
		{
			return;
		}
		for( auto& it: m_shapes )
		{
			it->advance();
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
		if( !m_active.load( std::memory_order_relaxed ) )
		{
			return;
		}
		for( auto& it: m_shapes )
		{
			it->paint();
		}
	}

	ShapeGroupEntry* ShapeGroup::insert( Shape* item )
	{
		ShapeList::Iterator it = m_shapes.append( item );
		return new ShapeGroupEntry( it );
	}

	ShapeGroupEntry::ShapeGroupEntry(
		ShapeGroup::ShapeList::Iterator const& iter )
		: m_iter( iter )
	{
	}

	ShapeGroupEntry::~ShapeGroupEntry()
	{
		m_iter.kill();
	}
}
