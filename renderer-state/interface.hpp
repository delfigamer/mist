#pragma once

#include <client-main/windowinfo.hpp>
#include <common/string.hpp>
#include <common.hpp>

#define LOG( format, ... ) \
	graphics::logwrite( \
		"[%73s:%4i]\t" format "\n", \
		__FILE__, __LINE__, ##__VA_ARGS__ )

namespace graphics
{
	extern window::WindowInfo* WindowInfo;

	void logwrite( char const* format, ... );
	ptrdiff_t getconfig_integer( char const* expr, ptrdiff_t def );
	double getconfig_number( char const* expr, double def );
	String getconfig_string( char const* expr, String const& def );
	bool getconfig_boolean( char const* expr, bool def );
}
