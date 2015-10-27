#ifndef UTILS_CBASE_HPP__
#define UTILS_CBASE_HPP__ 1

#include <exception>

#define CBASE_PROTECT( op ) \
		utils::cbase_seterror( 0 ); \
		try { \
			op \
		} catch( std::exception const& e ) { \
			utils::cbase_seterror( e.what() ); \
		} catch( ... ) { \
			utils::cbase_seterror( "unknown exception" ); \
		} \
		return 0;

namespace utils
{
	extern "C" {
		char const* cbase_geterror() noexcept;
		void cbase_seterror( char const* error ) noexcept;
		bool cbase_write( char const* str ) noexcept;
		bool cbase_getchar( char* str ) noexcept;
		void cbase_yield() noexcept;
	}
}

#endif
