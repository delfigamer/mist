#ifndef GRAPHICS_SHAPE_HPP__
#define GRAPHICS_SHAPE_HPP__ 1

#include "resource.hpp"

namespace graphics
{
	class Shape: public Resource
	{
	public:
		Shape();
		virtual ~Shape() override;
		Shape( Shape const& ) = delete;
		Shape& operator=( Shape const& ) = delete;

		virtual void paint() = 0;
	};
}

#endif
