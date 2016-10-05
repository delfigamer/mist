#pragma once

#include <common/string.hpp>
#include <common.hpp>
#include <lua/lua.hpp>
#include <cinttypes>

namespace utils
{
	class ConfClass;

	extern ConfClass* MainConf;

	R_CLASS( name = mainconf )
	class ConfClass
	{
	private:
		lua_State* m_lstate;

	public:
		ConfClass( char const* filename, char const* init );
		~ConfClass();
		ConfClass( ConfClass const& ) = delete;
		ConfClass& operator=( ConfClass const& ) = delete;

		bool binteger( char const* expr, ptrdiff_t* result );
		bool bnumber( char const* expr, double* result );
		bool bstring( char const* expr, String* result );
		bool bboolean( char const* expr, bool* result );
		bool stringbuf( char const* expr, char* buffer, size_t* length );
		ptrdiff_t integer( char const* expr, ptrdiff_t def );
		double number( char const* expr, double def );
		String string( char const* expr, String const& def );
		bool boolean( char const* expr, bool result );
		void runcmd( char const* expr );

		R_METHOD() static bool linteger(
			char const* expr, ptrdiff_t* value )
		{
			return MainConf->binteger( expr, value );
		}
		R_METHOD() static bool lnumber(
			char const* expr, double* value )
		{
			return MainConf->bnumber( expr, value );
		}
		R_METHOD() static bool lstringbuf(
			char const* expr, char* buffer, size_t* length )
		{
			return MainConf->stringbuf( expr, buffer, length );
		}
		R_METHOD() static bool lboolean(
			char const* expr, bool* value )
		{
			return MainConf->bboolean( expr, value );
		}
	};
}

/*
R_EMIT( target = lua_beforemetatypes )
function mainconf:integer(expr, def)
	local value = ffi.new('ptrdiff_t[1]')
	if self:linteger(expr, value) then
		return value[0]
	else
		return def
	end
end

function mainconf:number(expr, def)
	local value = ffi.new('double[1]')
	if self:lnumber(expr, value) then
		return value[0]
	else
		return def
	end
end

do
	local buffer = ffi.new('char[256]')
	local length = ffi.new('size_t[1]')
	function mainconf:string(expr, def)
		length[0] = 256
		local result = self:lstringbuf(expr, buffer, length)
		if not result then
			return def
		end
		local clen = tonumber(length[0])
		if clen > 256 then
			local largebuffer = ffi.new('char[?]', clen)
			self:lstringbuf(expr, largebuffer, length)
			return ffi.string(largebuffer, clen)
		else
			return ffi.string(buffer, clen)
		end
	end
end

function mainconf:boolean(expr, def)
	local value = ffi.new('bool[1]')
	if self:lboolean(expr, value) then
		return value[0]
	else
		return def
	end
end
R_END()
*/
