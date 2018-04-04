#pragma once

#include <client-main/windowinfo.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <string>

#define LOG( format, ... ) \
	graphics::logwrite( \
		"[%73s:%4i]\t" format "\n", \
		__FILE__, __LINE__, ##__VA_ARGS__ )

namespace graphics
{
	extern window::WindowInfo* WindowInfo;

	void logwrite( char const* format, ... );
	int getconfig_integer( char const* expr, int def = 0 );
	double getconfig_number( char const* expr, double def = 0 );
	Ref< DataBuffer > getconfig_data( char const* expr );
	std::string getconfig_string(
		char const* expr, std::string const& def = {} );
	bool getconfig_boolean( char const* expr, bool def = false );
}
