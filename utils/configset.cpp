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
		"function envsub(str)"
			"return string.gsub(str, '%%(.-)%%', getenv)"
		"end";

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
		template< typename T >
		T lua_get( lua_State* L, int index );

		template<>
		ptrdiff_t lua_get< ptrdiff_t >( lua_State* L, int index )
		{
			return lua_tointeger( L, index );
		}

		template<>
		double lua_get< double >( lua_State* L, int index )
		{
			return lua_tonumber( L, index );
		}

		template<>
		String lua_get< String >( lua_State* L, int index )
		{
			size_t len;
			char const* buf = lua_tolstring( L, index, &len );
			return String( buf, len );
		}

		template<>
		bool lua_get< bool >( lua_State* L, int index )
		{
			return lua_toboolean( L, index ) != 0;
		}

		template< typename T >
		bool accessor( lua_State* L, char const* expr, T* value )
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
						*value = lua_get< T >( L, -1 );
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
			T value;
			if( accessor< T >( L, expr, &value ) )
			{
				return value;
			}
			else
			{
				return def;
			}
		}
	}

	bool ConfClass::binteger( char const* expr, ptrdiff_t* value )
	{
		return accessor< ptrdiff_t >( m_lstate, expr, value );
	}

	bool ConfClass::bnumber( char const* expr, double* value )
	{
		return accessor< double >( m_lstate, expr, value );
	}

	bool ConfClass::bstring( char const* expr, String* value )
	{
		return accessor< String >( m_lstate, expr, value );
	}

	bool ConfClass::bboolean( char const* expr, bool* value )
	{
		return accessor< bool >( m_lstate, expr, value );
	}

	bool ConfClass::stringbuf(
		char const* expr, char* buffer, size_t* length )
	{
		String value;
		if( bstring( expr, &value ) )
		{
			if( length )
			{
				if( buffer )
				{
					size_t clen = value.getlength();
					if( clen <= *length )
					{
						memcpy( buffer, value.getchars(), clen );
					}
				}
				*length = value.getlength();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	ptrdiff_t ConfClass::integer( char const* expr, ptrdiff_t def )
	{
		return accessor_def< ptrdiff_t >( m_lstate, expr, def );
	}

	double ConfClass::number( char const* expr, double def )
	{
		return accessor_def< double >( m_lstate, expr, def );
	}

	String ConfClass::string( char const* expr, String const& def )
	{
		return accessor_def< String >( m_lstate, expr, def );
	}

	bool ConfClass::boolean( char const* expr, bool def )
	{
		return accessor_def< bool >( m_lstate, expr, def );
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
