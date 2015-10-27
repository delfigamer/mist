#include "cbase.hpp"
#include "console.hpp"
#include "string.hpp"
#include <cstdio>
#include <thread>

namespace utils
{
	extern "C"
	{
		static String cbase_error;

		char const* cbase_geterror() noexcept
		{
			DataBuffer* db = cbase_error.m_payload;
			if( db )
			{
				return ( char const* )db->m_data;
			}
			else
			{
				return 0;
			}
		}

		void cbase_seterror( char const* error ) noexcept
		{
			cbase_error.setchars( error );
		}

		bool cbase_write( char const* str ) noexcept
		{
		CBASE_PROTECT(
			Console()->write( "%s", str );
			return 1;
		)
		}

		bool cbase_getchar( char* str ) noexcept
		{
		CBASE_PROTECT(
			Console()->getchar( str );
			return 1;
		)
		}

		void cbase_yield() noexcept
		{
			std::this_thread::yield();
		}
	}
}
