#pragma once

#include <common/strexception.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class ExlError: public StrException
	{
	public:
		ExlError(
			char const* message, int sr, int sc, int er, int ec,
			char const* filename )
			: StrException( "%s:[%i:%i]-[%i:%i] %s",
				filename, sr, sc, er, ec, message )
		{
		}
		ExlError(
			char const* message, int sr, int sc,
			char const* filename )
			: StrException( "%s:[%i:%i] %s",
				filename, sr, sc, message )
		{
		}
		ExlError(
			char const* message, int sr, int sc, int er, int ec,
			char const* filename, int filenamelen )
			: StrException( "%.*s:[%i:%i]-[%i:%i] %s",
				filenamelen, filename, sr, sc, er, ec, message )
		{
		}
		ExlError( char const* message )
			: StrException( message )
		{
		}
		~ExlError()
		{
		}
	};
}
