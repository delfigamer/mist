#ifndef GRAPHICS_SHAPEGROUP_HPP__
#define GRAPHICS_SHAPEGROUP_HPP__ 1

#include "shape.hpp"
#include <utils/counterlock.hpp>
#include <utils/ref.hpp>
#include <mutex>
#include <atomic>
#include <map>

namespace graphics
{
	class ShapeGroup_iterator { ptrdiff_t p[ 8 ]; };

	class ShapeGroup: public Shape
	{
	private:
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		typedef std::multimap< int, utils::Ref< Shape > > items_t;

	private:
		mutex_t m_mutex;
		items_t m_items;
		std::atomic< bool > m_active;

	protected:
		virtual void doadvance(
			IDirect3DDevice9* device, int framecount ) override;

	public:
		class iterator: public items_t::iterator
		{
			using items_t::iterator::iterator;
		};

	public:
		ShapeGroup();
		ShapeGroup( Shape const& ) = delete;
		ShapeGroup( Shape&& ) = delete;
		virtual ~ShapeGroup() override;
		ShapeGroup& operator=( Shape const& ) = delete;
		ShapeGroup& operator=( Shape&& ) = delete;

		virtual void paint( IDirect3DDevice9* device ) override;
		void insert( Shape* item, int order, iterator* it );
		void remove( iterator* it );
		void setactive( bool active );
	};

	ShapeGroup* graphics_shapegroup_new() noexcept;
	bool graphics_shapegroup_insert(
		ShapeGroup* shape,
		Shape* item, int order, ShapeGroup_iterator* it ) noexcept;
	bool graphics_shapegroup_remove(
		ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept;
	bool graphics_shapegroup_setactive(
		ShapeGroup* shape, bool active ) noexcept;
}

#endif
