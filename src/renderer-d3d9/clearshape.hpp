#pragma once

#include <renderer-d3d9/shape.hpp>
#include <common.hpp>
#include <atomic>

namespace graphics
{
	R_CLASS( name = clearshape )
	class ClearShape: public Shape
	{
	private:
		std::atomic< int > m_flags;
		std::atomic< uint32_t > m_color;
		std::atomic< float > m_depth;
		std::atomic< uint32_t > m_stencil;

	protected:
		virtual void doadvance() override;

	public:
		ClearShape( bool ccolor, bool cdepth, bool cstencil );
		~ClearShape();
		ClearShape( ClearShape const& ) = delete;
		ClearShape& operator=( ClearShape const& ) = delete;

		virtual void paint() override;

		R_METHOD() static ClearShape* create(
			bool ccolor, bool cdepth, bool cstencil )
		{
			return new ClearShape( ccolor, cdepth, cstencil );
		}
		R_METHOD() void setcolor( float const* value );
		R_METHOD() void setdepth( float value );
		R_METHOD() void setstencil( uint32_t value );
	};
}
