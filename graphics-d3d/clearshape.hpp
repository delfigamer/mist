#ifndef GRAPHICS_CLEARSHAPE_HPP__
#define GRAPHICS_CLEARSHAPE_HPP__ 1

#include "shape.hpp"
#include <utils/flaglock.hpp>
#include <mutex>

namespace graphics
{
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
		void setcolor( bool flag, float const* value );
		void setdepth( bool flag, float value );
		void setstencil( bool flag, int value );
	};

	ClearShape* graphics_clearshape_new() noexcept;
	bool graphics_clearshape_setcolor(
		ClearShape* shape, float const* value ) noexcept;
	bool graphics_clearshape_setdepth(
		ClearShape* shape, float value ) noexcept;
	bool graphics_clearshape_setstencil(
		ClearShape* shape, int value ) noexcept;
}

#endif
