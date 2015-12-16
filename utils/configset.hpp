#ifndef UTILS_CONFIGSET_HPP__
#define UTILS_CONFIGSET_HPP__ 1

#include "string.hpp"
#include "console.hpp"
#include <lua/lua.hpp>

namespace utils
{
	class ConfigSet
	{
	private:
		lua_State* m_lstate;
		SingletonRef< ConsoleClass > m_console;

	public:
		ConfigSet() = delete;
		ConfigSet( char const* filename, char const* init = 0 );
		~ConfigSet();
		ConfigSet( ConfigSet const& other ) = delete;
		ConfigSet( ConfigSet&& other ) = delete;
		ConfigSet& operator=( ConfigSet const& other ) = delete;
		ConfigSet& operator=( ConfigSet&& other ) = delete;

		int integer( char const* expr, int def = 0 ) const;
		double number( char const* expr, double def = 0 ) const;
		String string(
			char const* expr, String const& def = 0 ) const;
		bool boolean( char const* expr, bool def = false ) const;
		void runcmd( char const* expr );
	};
}

#endif
