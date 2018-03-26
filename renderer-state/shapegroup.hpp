#pragma once

#include <renderer-state/shape.hpp>
#include <common/mpsclist.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>
#include <mutex>
#include <atomic>

namespace graphics
{
	class ShapeGroupEntry;

	R_CLASS( name = shapegroup )
	class ShapeGroup: public Shape
	{
	public:
		friend class ShapeGroupEntry;

	private:
		typedef MPSCList< Ref< Shape > > ShapeList;

	private:
		std::atomic< bool > m_active;
		ShapeList m_shapes;

	protected:
		virtual void doadvance() override;

	public:
		ShapeGroup();
		~ShapeGroup();

		virtual void paint() override;

		R_METHOD() static ShapeGroup* create()
		{
			return new ShapeGroup();
		}
		R_METHOD() ShapeGroupEntry* insert( Shape* item );
		R_METHOD() void setactive( bool active ) noexcept
		{
			m_active.store( active, std::memory_order_relaxed );
		}
	};

	R_CLASS( name = shapegroupentry )
	class ShapeGroupEntry: public RefObject
	{
	private:
		ShapeGroup::ShapeList::Iterator m_iter;

	public:
		ShapeGroupEntry( ShapeGroup::ShapeList::Iterator const& iter );
		~ShapeGroupEntry();
		ShapeGroupEntry( const ShapeGroupEntry& ) = delete;
		ShapeGroupEntry& operator=( const ShapeGroupEntry& ) = delete;
	};
}
