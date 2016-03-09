#ifndef GRAPHICS_SHAPEGROUP_HPP__
#define GRAPHICS_SHAPEGROUP_HPP__ 1

#include "shape.hpp"
#include "program.hpp"
#include <utils/ref.hpp>
#include <map>

namespace graphics {
	typedef struct { ptrdiff_t p[ 8 ]; } ShapeGroup_iterator;
	
	class ShapeGroup: public Shape {
	protected:
		typedef std::multimap< int, utils::Ref< Shape > > items_t;
		
		items_t m_items;
		utils::Ref< Program > m_program;
		
	public:
		class iterator: public items_t::iterator { using items_t::iterator::iterator; };
		
	public:
		virtual bool query( int id, void** target ) override;
		ShapeGroup();
		ShapeGroup( Shape const& ) = delete;
		ShapeGroup( Shape&& ) = delete;
		virtual ~ShapeGroup() override;
		ShapeGroup& operator=( Shape const& ) = delete;
		ShapeGroup& operator=( Shape&& ) = delete;
		
		virtual void paint() override;
		virtual void advance() override;
		void insert( Shape* item, int order, iterator* it );
		void remove( iterator* it );
		void begin( iterator* it );
		void end( iterator* it );
		void setprogram( Program* program );
	};
}

DEFINE_REFID( graphics::ShapeGroup, graphics::Shape, 0x91a45fc9 );

namespace graphics {
	extern "C" {
		ShapeGroup* graphics_shapegroup_new() noexcept;
		bool graphics_shapegroup_insert( ShapeGroup* shape, Shape* item, int order, ShapeGroup_iterator* it ) noexcept;
		bool graphics_shapegroup_remove( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept;
		bool graphics_shapegroup_begin( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept;
		bool graphics_shapegroup_end( ShapeGroup* shape, ShapeGroup_iterator* it ) noexcept;
		bool graphics_shapegroup_iterator_inc( ShapeGroup_iterator* it ) noexcept;
		int graphics_shapegroup_iterator_eq( ShapeGroup_iterator* a, ShapeGroup_iterator* b ) noexcept;
		bool graphics_shapegroup_iterator_deref( ShapeGroup_iterator* it, Shape** item, int* order ) noexcept;
		bool graphics_shapegroup_iterator_close( ShapeGroup_iterator* it ) noexcept;
		bool graphics_shapegroup_setprogram( ShapeGroup* shape, Program* program ) noexcept;
	}
}

#endif
