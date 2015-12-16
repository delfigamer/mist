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
		mutex_t m_colormutex;
		float m_color[ 4 ];

	public:
		ClearShape();
		virtual ~ClearShape() override;
		ClearShape( ClearShape const& ) = delete;
		ClearShape( ClearShape&& ) = delete;
		ClearShape& operator=( ClearShape const& ) = delete;
		ClearShape& operator=( ClearShape&& ) = delete;

		virtual void paint( IDirect3DDevice9* device ) override;
		void getcolor( float* color );
		void setcolor( float const* color );
	};

		ClearShape* graphics_clearshape_new() noexcept;
		bool graphics_clearshape_getcolor(
			ClearShape* shape, float* color ) noexcept;
		bool graphics_clearshape_setcolor(
			ClearShape* shape, float const* color ) noexcept;
}

#endif
