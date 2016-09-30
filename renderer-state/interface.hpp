#pragma once

#include <client-main/windowinfo.hpp>
#include <common/string.hpp>
#include <common.hpp>
#include <cstdint>

#define LOG( format, ... ) \
	graphics::logwrite( \
		"[%73s:%4i]\t" format "\n", \
		__FILE__, __LINE__, ##__VA_ARGS__ )

namespace graphics
{
	extern window::WindowInfo* WindowInfo;

	void logwrite( char const* format, ... );
	ptrdiff_t getconfig_integer( char const* expr, ptrdiff_t def = 0 );
	double getconfig_number( char const* expr, double def = 0 );
	size_t getconfig_lstring( char const* expr, void* buffer, size_t buflen );
	String getconfig_string(
		char const* expr, String const& def = 0 );
	bool getconfig_boolean( char const* expr, bool def = false );
}
