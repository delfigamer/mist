#ifndef UTILS_CONFIGSET_HPP__
#define UTILS_CONFIGSET_HPP__ 1

#include <utils/string.hpp>
#include <common.hpp>
#include <lua/lua.hpp>
#include <cinttypes>

namespace utils
{
	R_CLASS( name = configset )
	class ConfigSet
	{
	private:
		lua_State* m_lstate;

	public:
		ConfigSet() = delete;
		ConfigSet( char const* filename, char const* init = 0 );
		~ConfigSet();
		ConfigSet( ConfigSet const& ) = delete;
		ConfigSet& operator=( ConfigSet const& ) = delete;

		ptrdiff_t integer( char const* expr, ptrdiff_t def = 0 ) const;
		double number( char const* expr, double def = 0 ) const;
		String string(
			char const* expr, String const& def = 0 ) const;
		bool boolean( char const* expr, bool def = false ) const;
		void runcmd( char const* expr );
		R_METHOD( name = integer ) ptrdiff_t linteger( char const* expr, ptrdiff_t def ) const
		{
			return integer( expr, def );
		}
		R_METHOD( name = number ) double lnumber(
			char const* expr, double def ) const
		{
			return number( expr, def );
		}
		R_METHOD( name = lstring ) size_t lstring(
			char const* expr, char* buffer, size_t buflen ) const;
		R_METHOD( name = boolean ) bool lboolean(
			char const* expr, bool def ) const
		{
			return boolean( expr, def );
		}
	};
}

/*
R_EMIT( target = lua_beforemetatypes )
function configset:string(expr, def)
	local buflen = self:lstring(expr, nil, 0)
	if buflen == 0 then
		return def
	end
	local buffer = ffi.new('char[?]', buflen)
	self:lstring(expr, buffer, buflen)
	return ffi.string(buffer, buflen)
end
R_END()
*/

#endif
