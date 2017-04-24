#pragma once

#include <renderer-state/resource.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = shape )
	class Shape: public Resource
	{
	public:
		Shape();
		~Shape();

		virtual void paint() = 0;
	};
}
