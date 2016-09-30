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

	ptrdiff_t getconfig_integer( char const* expr, ptrdiff_t def )
	{
		ptrdiff_t ret;
		if( WindowInfo->client_methodlist->configset_integer(
			WindowInfo->configset, expr, def, &ret ) )
		{
			return ret;
		}
		else
		{
			char const* error;
			WindowInfo->client_methodlist->cbase_geterror( &error );
			throw StrException( "%s", error );
		}
	}

	double getconfig_number( char const* expr, double def )
	{
		double ret;
		if( WindowInfo->client_methodlist->configset_number(
			WindowInfo->configset, expr, def, &ret ) )
		{
			return ret;
		}
		else
		{
			char const* error;
			WindowInfo->client_methodlist->cbase_geterror( &error );
			throw StrException( "%s", error );
		}
	}

	size_t getconfig_lstring( char const* expr, void* buffer, size_t buflen )
	{
		size_t ret;
		if( WindowInfo->client_methodlist->configset_lstring(
			WindowInfo->configset, expr, ( char* )buffer, buflen, &ret ) )
		{
			return ret;
		}
		else
		{
			char const* error;
			WindowInfo->client_methodlist->cbase_geterror( &error );
			throw StrException( "%s", error );
		}
	}

	String getconfig_string( char const* expr, String const& def )
	{
		size_t len = getconfig_lstring( expr, 0, 0 );
		if( len == 0 )
		{
			return def;
		}
		auto payload = DataBuffer::create( len, len, 0 );
		payload->m_length =
			getconfig_lstring( expr, payload->m_data, payload->m_capacity );
		return String( std::move( payload ) );
	}

	bool getconfig_boolean( char const* expr, bool def )
	{
		bool ret;
		if( WindowInfo->client_methodlist->configset_boolean(
			WindowInfo->configset, expr, def, &ret ) )
		{
			return ret;
		}
		else
		{
			char const* error;
			WindowInfo->client_methodlist->cbase_geterror( &error );
			throw StrException( "%s", error );
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
