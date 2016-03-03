#ifndef GRAPHICS_CLEARSHAPE_HPP__
#define GRAPHICS_CLEARSHAPE_HPP__ 1

#include "shape.hpp"
#include <utils/flaglock.hpp>
#include <common.hpp>
#include <mutex>

namespace graphics
{
	R_CLASS( name = clearshape )
	class ClearShape: public Shape
	{
	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		int m_flags;
		uint32_t m_color;
		float m_depth;
		int m_stencil;

	protected:
		virtual void doadvance() override;

	public:
		ClearShape();
		virtual ~ClearShape() override;
		ClearShape( ClearShape const& ) = delete;
		ClearShape& operator=( ClearShape const& ) = delete;

		virtual void paint() override;

		R_METHOD() static ClearShape* create()
		{
			return new ClearShape();
		}
		R_METHOD() void setcolor( bool flag, float const* value );
		R_METHOD() void setdepth( bool flag, float value );
		R_METHOD() void setstencil( bool flag, int value );
	};
}

#endif
