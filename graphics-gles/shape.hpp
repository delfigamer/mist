#ifndef GRAPHICS_SHAPE_HPP__
#define GRAPHICS_SHAPE_HPP__ 1

#include "resource.hpp"

namespace graphics
{
	class Shape: public Resource
	{
	protected:
		virtual void doadvance( int framecount ) override;

	public:
		Shape();
		virtual ~Shape() override;
		Shape( Shape const& ) = delete;
		Shape& operator=( Shape const& ) = delete;

		virtual void paint();
	};
}

#endif
