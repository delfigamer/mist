#ifndef GRAPHICS_LINESHAPE_HPP__
#define GRAPHICS_LINESHAPE_HPP__ 1

#include "coloredshape.hpp"

namespace graphics {
	class LineShape: public ColoredShape {
	private:
		GLfloat m_pos[6];
		
	public:
		virtual bool query( int id, void** target ) override;
		LineShape();
		LineShape( LineShape const& ) = delete;
		LineShape( LineShape&& ) = delete;
		virtual ~LineShape();
		LineShape& operator=( LineShape const& ) = delete;
		LineShape& operator=( LineShape&& ) = delete;
		
		virtual void paint() override;
		void getpos( GLfloat* pos );
		void setpos( GLfloat const* pos );
	};
}

DEFINE_REFID( graphics::LineShape, graphics::ColoredShape, 0xb7ed546e )

namespace graphics {
	extern "C" {
		LineShape* graphics_lineshape_new() noexcept;
		bool graphics_lineshape_getpos( LineShape* shape, GLfloat* pos ) noexcept;
		bool graphics_lineshape_setpos( LineShape* shape, GLfloat const* pos ) noexcept;
	}
}

#endif
