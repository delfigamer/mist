#ifndef GRAPHICS_SHAPE_HPP__
#define GRAPHICS_SHAPE_HPP__ 1

#include "resource.hpp"
#include <d3d9.h>

namespace graphics {
	class Shape: public Resource {
	protected:
		virtual void doadvance( IDirect3DDevice9* device, int framecount ) override;
		
	public:
		virtual bool query( int id, void** target ) override;
		Shape();
		Shape( Shape const& ) = delete;
		Shape( Shape&& ) = delete;
		virtual ~Shape() override;
		Shape& operator=( Shape const& ) = delete;
		Shape& operator=( Shape&& ) = delete;
		
		virtual void paint( IDirect3DDevice9* device );
	};
}

DEFINE_REFID( graphics::Shape, utils::Monitor, 0x2bd805ca )

namespace graphics {
	extern "C" {
	}
}

#endif
