#pragma once

#if !defined( _WIN32 ) && !defined( _WIN64 )
#error "Unsupported target platform"
#endif

#include <renderer-d3d9/shape.hpp>
#include <common/ref.hpp>
#include <d3d9.h>
#include <windows.h>

namespace graphics
{
	class Display
	{
	private:
		HWND m_hwnd;
		AtomicRef< graphics::Shape > m_shape;

	public:
		Display( HWND hwnd );
		~Display();
		Display( Display const& ) = delete;
		Display& operator=( Display const& ) = delete;

		void paint();
		void setshape( Shape* nv );
	};
}
