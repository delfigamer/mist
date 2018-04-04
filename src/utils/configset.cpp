#include <utils/configset.hpp>
#include <utils/console.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <utils/encoding.hpp>
#elif defined( __ANDROID__ )
#endif
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <osapi.hpp>
#include <cstring>

namespace utils
{
	int lua_getenv( lua_State* L )
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		char const* name = lua_tostring( L, 1 );
		Ref< DataBuffer > wname = translatebuffer(
			&encoding::utf8, &encoding::utf16, name );
		int buflen = GetEnvironmentVariableW( LPCWSTR( wname->m_data ), 0, 0 );
		if( buflen == 0 )
		{
			lua_pushstring( L, "" );
			return 1;
		}
		Ref< DataBuffer > wvalue = DataBuffer::create( buflen * 2 + 2 );
		buflen = GetEnvironmentVariableW(
			LPCWSTR( wname->m_data ), LPWSTR( wvalue->m_data ), buflen + 1 );
		Ref< DataBuffer > value = translatebuffer(
			&encoding::utf16, &encoding::utf8, wvalue );
		lua_pushstring( L, ( char const* )value->m_data );
		return 1;
#elif defined( __ANDROID__ )
		ASSERT( false );
#endif
	}

	char const* luadef_envsub =
		"function envsub(str)\
			return string.gsub(str, '%%(.-)%%', getenv)\
		end";

	ConfClass::ConfClass( char const* filename, char const* init )
	{
		m_lstate = luaL_newstate();
		lua_pushcfunction( m_lstate, &luaopen_string );
		lua_call( m_lstate, 0, 0 );
		lua_pushcfunction( m_lstate, &lua_getenv );
		lua_setglobal( m_lstate, "getenv" );
		luaL_loadstring( m_lstate, luadef_envsub );
		lua_call( m_lstate, 0, 0 );
		if( init )
		{
			if( luaL_loadstring( m_lstate, init ) != 0 )
			{
				LOG( "! failed to load initialization string \"%s\": %s",
					init, lua_tostring( m_lstate, -1 ) );
				goto fail;
			}
			if( lua_pcall( m_lstate, 0, 0, 0 ) != 0 )
			{
				LOG( "! failed to run initialization string \"%s\": %s",
					init, lua_tostring( m_lstate, -1 ) );
				goto fail;
			}
		}
		if( luaL_loadfile( m_lstate, filename ) != 0 )
		{
			LOG( "! failed to load file \"%s\": %s",
				filename, lua_tostring( m_lstate, -1 ) );
			goto fail;
		}
		if( lua_pcall( m_lstate, 0, 0, 0 ) != 0 )
		{
			LOG( "! failed to run file \"%s\": %s",
				filename, lua_tostring( m_lstate, -1 ) );
			goto fail;
		}
		return;

	fail:
		lua_settop( m_lstate, 0 );
	}

	ConfClass::~ConfClass()
	{
		lua_close( m_lstate );
	}

	namespace
	{
		void lua_get( lua_State* L, int index, int& result )
		{
			result = lua_tointeger( L, index );
		}

		void lua_get( lua_State* L, int index, double& result )
		{
			result = lua_tonumber( L, index );
		}

		void lua_get( lua_State* L, int index, Ref< DataBuffer >& result )
		{
			size_t len;
			char const* buf = lua_tolstring( L, index, &len );
			if( len != 0 )
			{
				result = DataBuffer::create( len, buf );
			}
			else
			{
				result = nullptr;
			}
		}

		void lua_get( lua_State* L, int index, std::string& result )
		{
			size_t len;
			char const* buf = lua_tolstring( L, index, &len );
			result = std::string( buf, buf + len );
		}

		void lua_get( lua_State* L, int index, bool& result )
		{
			result = lua_toboolean( L, index ) != 0;
		}

		struct ByteRange
		{
			char* start;
			size_t length;
		};

		void lua_get( lua_State* L, int index, ByteRange& range )
		{
			size_t len;
			char const* buf = lua_tolstring( L, index, &len );
			if( len <= range.length )
			{
				memcpy( range.start, buf, len );
			}
			range.length = len;
		}

		template< typename T >
		bool accessor( lua_State* L, char const* expr, T& value )
		{
			bool result = false;
			lua_pushstring( L, "return " );
			lua_pushstring( L, expr );
			lua_concat( L, 2 );
			size_t len;
			char const* buf = lua_tolstring( L, -1, &len );
			if( luaL_loadbuffer( L, buf, len, expr ) != 0 )
			{
				LOG( "! failed to access expression \"%s\": %s",
					expr, lua_tostring( L, -1 ) );
			}
			else
			{
				if( lua_pcall( L, 0, 1, 0 ) == 0 )
				{
					if( !lua_isnil( L, -1 ) )
					{
						lua_get( L, -1, value );
						result = true;
					}
				}
			}
			lua_settop( L, 0 );
			return result;
		}

		template< typename T >
		T accessor_def( lua_State* L, char const* expr, T const& def )
		{
			T result;
			if( accessor< T >( L, expr, result ) )
			{
				return result;
			}
			else
			{
				return def;
			}
		}
	}

	bool ConfClass::binteger( char const* expr, int& value )
	{
		return accessor( m_lstate, expr, value );
	}

	bool ConfClass::bnumber( char const* expr, double& value )
	{
		return accessor( m_lstate, expr, value );
	}

	bool ConfClass::bdata( char const* expr, Ref< DataBuffer >& value )
	{
		return accessor( m_lstate, expr, value );
	}

	bool ConfClass::bstring( char const* expr, std::string& value )
	{
		return accessor( m_lstate, expr, value );
	}

	bool ConfClass::bboolean( char const* expr, bool& value )
	{
		return accessor( m_lstate, expr, value );
	}

	bool ConfClass::stringbuf(
		char const* expr, char* buffer, size_t& length )
	{
		ByteRange range{ buffer, length };
		if( accessor( m_lstate, expr, range ) )
		{
			length = range.length;
			return true;
		}
		else
		{
			return false;
		}
	}

	int ConfClass::integer( char const* expr, int def )
	{
		return accessor_def( m_lstate, expr, def );
	}

	double ConfClass::number( char const* expr, double def )
	{
		return accessor_def( m_lstate, expr, def );
	}

	Ref< DataBuffer > ConfClass::data( char const* expr )
	{
		return accessor_def( m_lstate, expr, Ref< DataBuffer >( nullptr ) );
	}

	std::string ConfClass::string( char const* expr, std::string const& def )
	{
		return accessor_def( m_lstate, expr, def );
	}

	bool ConfClass::boolean( char const* expr, bool def )
	{
		return accessor_def( m_lstate, expr, def );
	}

	void ConfClass::runcmd( char const* expr )
	{
		if( luaL_loadstring( m_lstate, expr ) != 0 )
		{
			LOG( "! failed to load command \"%s\": %s",
				expr, lua_tostring( m_lstate, -1 ) );
		}
		else
		{
			if( lua_pcall( m_lstate, 0, 1, 0 ) != 0 )
			{
				LOG( "! failed to run command \"%s\": %s",
					expr, lua_tostring( m_lstate, -1 ) );
			}
		}
		lua_settop( m_lstate, 0 );
	}

	ConfClass* MainConf;
}
