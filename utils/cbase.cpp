#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <utils/string.hpp>
#include <thread>

namespace utils
{
	static String cbase_error;

	char const* cbase::geterror() NOEXCEPT
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

	void cbase::seterror( char const* error ) NOEXCEPT
	{
		cbase_error.setchars( error );
	}

	void cbase::yield() NOEXCEPT
	{
		std::this_thread::yield();
	}

	void cbase::write( char const* str )
	{
		Console->write( "%s", str );
	}

	void cbase::getchar( char* str )
	{
		Console->getchar( str );
	}

}
