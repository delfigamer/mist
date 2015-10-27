#ifndef GRAPHICS_CLEARSHAPE_HPP__
#define GRAPHICS_CLEARSHAPE_HPP__ 1

#include "coloredshape.hpp"

namespace graphics {
	class ClearShape: public ColoredShape {
	public:
		virtual bool query( int id, void** target ) override;
		ClearShape();
		ClearShape( ClearShape const& ) = delete;
		ClearShape( ClearShape&& ) = delete;
		virtual ~ClearShape();
		ClearShape& operator=( ClearShape const& ) = delete;
		ClearShape& operator=( ClearShape&& ) = delete;
		
		virtual void paint( IDirect3DDevice9* device ) override;
	};
}

DEFINE_REFID( graphics::ClearShape, graphics::ColoredShape, 0x0f991f45 )

namespace graphics {
	extern "C" {
		ClearShape* graphics_clearshape_new() noexcept;
	}
}

#endif
