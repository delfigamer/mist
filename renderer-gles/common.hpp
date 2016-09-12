#pragma once

#include <renderer-gles/context.hpp>
#include <renderer-gles/interface.hpp>

/*
R_EMIT( typename =
	class ::utils::RefObject | refobject |
	class ::utils::DataBuffer | databuffer )
R_END()
R_EMIT( target = hpp_beforeclasses )
	namespace utils {class RefObject;}
	namespace utils {class DataBuffer;}
R_END()
R_EMIT( target = cpp_start )
	namespace utils
	{
		struct cbase
		{
			static void seterror( char const* error );
		};
	}
	namespace utils {class DataBuffer;}
R_END()
R_EMIT( target = lua_beforeclasses )
	local methodlist = ffi.cast(
		'struct renderer_gles_methodlist_t const*',
		require('host.info').renderer_methodlist)
	local cbase = require('host.cbase')
	local refobject = require('host.refobject')
	local databuffer = require('host.databuffer')
R_END()
R_EMIT( target = lua_beforemethods )
local client_methodlist = require('host.info').client_methodlist
local function reference_addref(self)
	if self ~= nil then
		client_methodlist.refobject_addref(self)
	end
end
R_END()
*/

#include <renderer-state/common.hpp>

namespace graphics
{
	void checkerror_pos(
		char const* filename, char const* function, int line );
}

#define checkerror() checkerror_pos( __FILE__, __FUNCTION__, __LINE__ )
