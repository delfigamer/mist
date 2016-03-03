#ifndef GRAPHICS_SHAPEGROUP_HPP__
#define GRAPHICS_SHAPEGROUP_HPP__ 1

#include "shape.hpp"
#include <utils/flaglock.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <common.hpp>
#include <mutex>
#include <atomic>
#include <map>

namespace graphics
{
	class ShapeGroupEntry;

	class ShapeGroup: public Shape
	{
	public:
		friend class ShapeGroupEntry;

	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		typedef std::multimap< int, utils::Ref< Shape > > items_t;

	private:
		mutex_t m_mutex;
		items_t m_items;
		std::atomic< bool > m_active;

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
		R_METHOD() ShapeGroupEntry* insert( Shape* item, int order );
		R_METHOD() void setactive( bool active );
	};

	R_CLASS( name = shapegroupentry )
	class ShapeGroupEntry: public utils::RefObject
	{
	private:
		utils::Ref< ShapeGroup > m_shapegroup;
		ShapeGroup::items_t::iterator m_iter;

	public:
		ShapeGroupEntry(
			ShapeGroup* sg, ShapeGroup::items_t::iterator const& iter );
		~ShapeGroupEntry();
		ShapeGroupEntry( const ShapeGroupEntry& ) = delete;
		ShapeGroupEntry& operator=( const ShapeGroupEntry& ) = delete;
	};
}

#endif
