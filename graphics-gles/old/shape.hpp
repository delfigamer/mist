#ifndef GRAPHICS_SHAPE_HPP__
#define GRAPHICS_SHAPE_HPP__ 1

#include "opengl.hpp"
#include <utils/monitor.hpp>

namespace graphics {
	class Shape: public utils::Monitor {
	public:
		virtual bool query( int id, void** target ) override;
		Shape();
		Shape( Shape const& ) = delete;
		Shape( Shape&& ) = delete;
		virtual ~Shape() override;
		Shape& operator=( Shape const& ) = delete;
		Shape& operator=( Shape&& ) = delete;
		
		virtual void paint();
		virtual void advance();
	};
}

DEFINE_REFID( graphics::Shape, utils::Monitor, 0x2bd805ca )

namespace graphics {
	extern "C" {
	}
}

#endif
