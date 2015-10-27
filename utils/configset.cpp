#include "configset.hpp"
#include "console.hpp"

namespace utils {
	ConfigSet::ConfigSet( char const* filename, char const* init ) :
		m_lstate( luaL_newstate() ) {
		if( init ) {
			if( luaL_loadstring( m_lstate, init ) != 0 ) {
				LOG(
					"! failed to load initialization string \"%s\": %s",
					init, lua_tostring( m_lstate, -1 ) );
				goto fail;
			}
			if( lua_pcall( m_lstate, 0, 0, 0 ) != 0 ) {
				LOG(
					"! failed to run initialization string \"%s\": %s",
					init, lua_tostring( m_lstate, -1 ) );
				goto fail;
			}
		}
		if( luaL_loadfile( m_lstate, filename ) != 0 ) {
			LOG(
				"! failed to load file \"%s\": %s",
				filename, lua_tostring( m_lstate, -1 ) );
			goto fail;
		}
		if( lua_pcall( m_lstate, 0, 0, 0 ) != 0 ) {
			LOG(
				"! failed to run file \"%s\": %s",
				filename, lua_tostring( m_lstate, -1 ) );
			goto fail;
		}
		return;

	fail:
		lua_settop( m_lstate, 0 );
	}

	ConfigSet::~ConfigSet() {
		lua_close( m_lstate );
	}

#define ConfigSet_accessor( type, method ) {\
	type result = def; \
	lua_pushstring( m_lstate, "return " ); \
	lua_pushstring( m_lstate, expr ); \
	lua_concat( m_lstate, 2 ); \
	size_t len; \
	char const* buf = lua_tolstring( m_lstate, -1, &len ); \
	if( luaL_loadbuffer( m_lstate, buf, len, expr ) != 0 ) { \
		LOG( \
			"! failed to access expression \"%s\": %s", \
			expr, lua_tostring( m_lstate, -1 ) ); \
		result = def; \
	} else { \
		if( lua_pcall( m_lstate, 0, 1, 0 ) == 0 ) { \
			if( lua_isnil( m_lstate, -1 ) ) { \
				result = def; \
			} else { \
				result = ( method )( m_lstate, -1 ); \
			} \
		} else { \
			result = def; \
		} \
	} \
	lua_settop( m_lstate, 0 ); \
	return result; \
}

	int ConfigSet::integer( char const* expr, int def ) const {
		ConfigSet_accessor( int, lua_tointeger );
	}

	double ConfigSet::number( char const* expr, double def ) const {
		ConfigSet_accessor( double, lua_tonumber );
	}

	String ConfigSet::string(
			char const* expr, String const& def ) const {
		String result;
		lua_pushstring( m_lstate, "return " );
		lua_pushstring( m_lstate, expr );
		lua_concat( m_lstate, 2 );
		size_t len;
		char const* buf = lua_tolstring( m_lstate, -1, &len );
		if( luaL_loadbuffer( m_lstate, buf, len, expr ) != 0 ) {
			LOG(
				"! failed to access expression \"%s\": %s",
				expr, lua_tostring( m_lstate, -1 ) );
			result = def;
		} else {
			if( lua_pcall( m_lstate, 0, 1, 0 ) == 0 ) {
				if( lua_isnil( m_lstate, -1 ) ) {
					result = def;
				} else {
					buf = lua_tolstring( m_lstate, -1, &len );
					result.setchars( buf, len );
				}
			} else {
				result = def;
			}
		}
		lua_settop( m_lstate, 0 );
		return result;
	}

	bool ConfigSet::boolean( char const* expr, bool def ) const {
		ConfigSet_accessor( bool, lua_toboolean );
	}

	void ConfigSet::runcmd( char const* expr ) {
		lua_pushstring( m_lstate, expr );
		size_t len;
		char const* buf = lua_tolstring( m_lstate, -1, &len );
		if( luaL_loadbuffer( m_lstate, buf, len, expr ) != 0 ) {
			LOG(
				"! failed to load command \"%s\": %s",
				expr, lua_tostring( m_lstate, -1 ) );
		} else {
			if( lua_pcall( m_lstate, 0, 1, 0 ) != 0 ) {
				LOG(
					"! failed to run command \"%s\": %s",
					expr, lua_tostring( m_lstate, -1 ) );
			}
		}
		lua_settop( m_lstate, 0 );
	}
}
