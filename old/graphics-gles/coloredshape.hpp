#ifndef GRAPHICS_COLOREDSHAPE_HPP__
#define GRAPHICS_COLOREDSHAPE_HPP__ 1

#include "shape.hpp"

namespace graphics {
	class ColoredShape: public Shape {
	private:
		float m_color[ 4 ];
		
	public:
		virtual bool query( int id, void** target ) override;
		ColoredShape();
		explicit ColoredShape( float const* color );
		ColoredShape( ColoredShape const& ) = delete;
		ColoredShape( ColoredShape&& ) = delete;
		virtual ~ColoredShape();
		ColoredShape& operator=( ColoredShape const& ) = delete;
		ColoredShape& operator=( ColoredShape&& ) = delete;
		
		void getcolor( float* color );
		void setcolor( float const* color );
	};
}

DEFINE_REFID( graphics::ColoredShape, graphics::Shape, 0x2fe14ca5 )

namespace graphics {
	extern "C" {
		bool graphics_coloredshape_getcolor( ColoredShape* shape, float* color ) noexcept;
		bool graphics_coloredshape_setcolor( ColoredShape* shape, float const* color ) noexcept;
	}
}

#endif
