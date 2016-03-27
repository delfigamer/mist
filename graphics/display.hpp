#ifndef GRAPHICS_DISPLAY_HPP__
#define GRAPHICS_DISPLAY_HPP__ 1

#if !defined( _WIN32 ) && !defined( _WIN64 )
#error "Unsupported target platform"
#endif

#include <graphics/shape.hpp>
#include <utils/configset.hpp>
#include <utils/ref.hpp>
#include <d3d9.h>
#include <windows.h>

namespace graphics
{
	struct DisplayInfo
	{
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
	};

	class Display
	{
	private:
		HWND m_hwnd;
		utils::Ref< graphics::Shape > m_shape;
		DisplayInfo m_info;

	public:
		Display();
		~Display();
		Display( Display const& ) = delete;
		Display& operator=( Display const& ) = delete;

		void initialize( utils::ConfigSet const& config, HWND hwnd );
		void finalize();
		void paint();
		DisplayInfo const* getdisplayinfo();
		void setshape( Shape* nv );
	};
}

#endif
