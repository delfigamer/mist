#pragma once

#include <renderer-state/shape.hpp>
#include <utils/mpsclist.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
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
		typedef utils::MPSCList< utils::Ref< Shape > > ShapeList;

	private:
		std::atomic< bool > m_active;
		ShapeList m_shapes;

	protected:
		virtual void doadvance() override;

	public:
		ShapeGroup();
		virtual ~ShapeGroup() override;
		ShapeGroup( Shape const& ) = delete;
		ShapeGroup& operator=( Shape const& ) = delete;

		virtual void paint() override;

		R_METHOD() static ShapeGroup* create()
		{
			return new ShapeGroup();
		}
		R_METHOD() ShapeGroupEntry* insert( Shape* item );
		R_METHOD() void setactive( bool active ) NOEXCEPT
		{
			m_active.store( active, std::memory_order_relaxed );
		}
	};

	R_CLASS( name = shapegroupentry )
	class ShapeGroupEntry: public utils::RefObject
	{
	private:
		utils::Ref< ShapeGroup > m_shapegroup;
		ShapeGroup::ShapeList::Iterator m_iter;

	public:
		ShapeGroupEntry( ShapeGroup::ShapeList::Iterator const& iter );
		~ShapeGroupEntry();
		ShapeGroupEntry( const ShapeGroupEntry& ) = delete;
		ShapeGroupEntry& operator=( const ShapeGroupEntry& ) = delete;
	};
}
