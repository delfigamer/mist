#pragma once

#include <client-main/methodlist.hpp>
#include <common/strexception.hpp>
#include <common/databuffer.hpp>
#include <utility>
#include <cstdarg>
#include <cstdio>

#if defined( _WIN32 ) || defined( _WIN64 )
#define EXPORT __declspec( dllexport )
#endif

namespace graphics
{
	window::WindowInfo* WindowInfo;

	class Shape;
}

namespace utils
{
	struct cbase
	{
		static void seterror( char const* error );
	};

	void cbase::seterror( char const* error )
	{
		graphics::WindowInfo->client_methodlist->cbase_seterror( error );
	}
}

namespace graphics
{
	void logwrite( char const* format, ... )
	{
		va_list args;
		va_start( args, format );
		va_list args2;
		va_copy( args2, args );
		int buflen = vsnprintf( nullptr, 0, format, args2 );
		if( buflen < 1023 )
		{
			char buffer[ 1024 ];
			vsnprintf( buffer, sizeof( buffer ), format, args );
			WindowInfo->client_methodlist->cbase_write( buffer );
		}
		else
		{
			char* buffer = new char[ buflen + 1 ];
			vsnprintf( buffer, buflen + 1, format, args );
			WindowInfo->client_methodlist->cbase_write( buffer );
			delete[] buffer;
		}
	}

#define PROTECTED_CALL_RA( func, rt, ... ) \
	rt ret; \
	if( WindowInfo->client_methodlist->func( \
		__VA_ARGS__, &ret ) ) \
	{ \
		return ret; \
	} \
	else \
	{ \
		char const* error; \
		WindowInfo->client_methodlist->cbase_geterror( &error ); \
		throw StrException( "%s", error ); \
	}

	namespace
	{
		bool mainconf_linteger( char const* expr, ptrdiff_t* value )
		{
			PROTECTED_CALL_RA( mainconf_linteger, bool, expr, value )
		}

		bool mainconf_lnumber( char const* expr, double* value )
		{
			PROTECTED_CALL_RA( mainconf_lnumber, bool, expr, value )
		}

		bool mainconf_stringbuf( char const* expr, char* buffer, size_t* length )
		{
			PROTECTED_CALL_RA( mainconf_lstringbuf, bool, expr, buffer, length )
		}

		bool mainconf_lboolean( char const* expr, bool* value )
		{
			PROTECTED_CALL_RA( mainconf_lboolean, bool, expr, value )
		}
	}

	ptrdiff_t getconfig_integer( char const* expr, ptrdiff_t def )
	{
		ptrdiff_t value;
		if( mainconf_linteger( expr, &value ) )
		{
			return value;
		}
		else
		{
			return def;
		}
	}

	double getconfig_number( char const* expr, double def )
	{
		double value;
		if( mainconf_lnumber( expr, &value ) )
		{
			return value;
		}
		else
		{
			return def;
		}
	}

	String getconfig_string( char const* expr, String const& def )
	{
		size_t length;
		if( mainconf_stringbuf( expr, 0, &length ) )
		{
			auto payload = DataBuffer::create( length + 1 );
			mainconf_stringbuf( expr, ( char* )payload->m_data, &length );
			payload->m_data[ length ] = 0;
			return String( std::move( payload ) );
		}
		else
		{
			return def;
		}
	}

	bool getconfig_boolean( char const* expr, bool def )
	{
		bool value;
		if( mainconf_lboolean( expr, &value ) )
		{
			return value;
		}
		else
		{
			return def;
		}
	}

	extern "C" EXPORT bool renderer_display_create(
		HWND hwnd, Display** display )
	{
		try
		{
			*display = new Display( hwnd );
			return true;
		}
		catch( std::exception const& e )
		{
			utils::cbase::seterror( e.what() );
			return false;
		}
		catch( ... )
		{
			utils::cbase::seterror( "unknown error" );
			return false;
		}
	}

	extern "C" EXPORT bool renderer_display_destroy( Display* display )
	{
		try
		{
			delete display;
			return true;
		}
		catch( std::exception const& e )
		{
			utils::cbase::seterror( e.what() );
			return false;
		}
		catch( ... )
		{
			utils::cbase::seterror( "unknown error" );
			return false;
		}
	}

	extern "C" EXPORT bool renderer_display_paint( Display* display )
	{
		try
		{
			display->paint();
			return true;
		}
		catch( std::exception const& e )
		{
			utils::cbase::seterror( e.what() );
			return false;
		}
		catch( ... )
		{
			utils::cbase::seterror( "unknown error" );
			return false;
		}
	}

	extern "C" EXPORT bool renderer_display_setshape(
		Display* display, Shape* shape )
	{
		try
		{
			display->setshape( shape );
			return true;
		}
		catch( std::exception const& e )
		{
			utils::cbase::seterror( e.what() );
			return false;
		}
		catch( ... )
		{
			utils::cbase::seterror( "unknown error" );
			return false;
		}
	}
}
