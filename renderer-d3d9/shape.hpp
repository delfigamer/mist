#pragma once

#include <renderer-d3d9/resource.hpp>
#include <common.hpp>

namespace graphics
{
	R_CLASS( name = shape )
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
