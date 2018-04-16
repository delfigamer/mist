#pragma once

#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <lua/lua.hpp>
#include <string>
#include <memory>

namespace utils
{
	class ConfClass;

	extern std::unique_ptr< ConfClass > MainConf;

	class [[ r::class, r::name( "mainconf" ) ]] ConfClass
	{
	private:
		lua_State* m_lstate;

	public:
		ConfClass( char const* filename, char const* init );
		~ConfClass();
		ConfClass( ConfClass const& ) = delete;
		ConfClass& operator=( ConfClass const& ) = delete;

		bool binteger( char const* expr, int& result );
		bool bnumber( char const* expr, double& result );
		bool bdata( char const* expr, Ref< DataBuffer >& result );
		bool bstring( char const* expr, std::string& result );
		bool bboolean( char const* expr, bool& result );
		bool stringbuf( char const* expr, char* buffer, size_t& length );
		int integer( char const* expr, int def = 0 );
		double number( char const* expr, double def = 0 );
		Ref< DataBuffer > data( char const* expr );
		std::string string( char const* expr, std::string const& def = {} );
		bool boolean( char const* expr, bool def = false );
		void runcmd( char const* expr );

		[[ r::method ]] static bool linteger(
			char const* expr , int* value  )
		{
			return MainConf->binteger( expr, *value );
		}

		[[ r::method ]] static bool lnumber(
			char const* expr , double* value  )
		{
			return MainConf->bnumber( expr, *value );
		}

		[[ r::method ]] static bool lstringbuf(
			char const* expr ,
			char* buffer, size_t* length  )
		{
			return MainConf->stringbuf( expr, buffer, *length );
		}

		[[ r::method ]] static bool lboolean(
			char const* expr , bool* value  )
		{
			return MainConf->bboolean( expr, *value );
		}
	};
}

r_emit(<<
function utils_mainconf:integer(expr, def)
	local value = ffi.new('int[1]')
	if self:linteger(expr, value) then
		return value[0]
	else
		return def
	end
end

function utils_mainconf:number(expr, def)
	local value = ffi.new('double[1]')
	if self:lnumber(expr, value) then
		return value[0]
	else
		return def
	end
end

do
	local buffer = ffi.new('char[1024]')
	local length = ffi.new('size_t[1]')
	function utils_mainconf:string(expr, def)
		length[0] = 1024
		local result = self:lstringbuf(expr, buffer, length)
		if not result then
			return def
		end
		local clen = tonumber(length[0])
		if clen > 1024 then
			local largebuffer = ffi.new('char[?]', clen)
			self:lstringbuf(expr, largebuffer, length)
			return ffi.string(largebuffer, clen)
		else
			return ffi.string(buffer, clen)
		end
	end
end

function utils_mainconf:boolean(expr, def)
	local value = ffi.new('bool[1]')
	if self:lboolean(expr, value) then
		return value[0]
	else
		return def
	end
end
>>)
