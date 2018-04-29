#pragma once

#include <common.hpp>
#include <mutex>
#include <memory>
#include <cstdarg>
#if defined( getchar )
#undef getchar
#endif

namespace utils
{
	class ConsoleClass
	{
	private:
		typedef std::recursive_mutex mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		void* m_file;
		bool m_newline;
#if defined( _WIN32 ) || defined( _WIN64 )
		void* m_inputhandle;
		bool m_inputisconsole;
		void* m_outputhandle;
		bool m_outputisconsole;
#endif
		uint32_t m_newline_consumed = 0;

		void writeconsole( char const* str, size_t length );
		void writefile( char const* str, size_t length );
		void linestart();

	public:
		ConsoleClass();
		~ConsoleClass();
		ConsoleClass( ConsoleClass const& ) = delete;
		ConsoleClass& operator=( ConsoleClass const& ) = delete;

		void writeraw( char const* str );
		__attribute__(( __format__( gnu_printf, 2, 0 ) ))
		void vwrite( char const* format, va_list args );
		__attribute__(( __format__( gnu_printf, 2, 3 ) ))
		void write( char const* format, ... );
		__attribute__(( __format__( gnu_printf, 2, 0 ) ))
		void vwriteln( char const* format, va_list args );
		__attribute__(( __format__( gnu_printf, 2, 3 ) ))
		void writeln( char const* format, ... );
		void writeln();
		void flush();
		void lock();
		void unlock();
		void getchar( char* str );
	};

	extern std::unique_ptr< ConsoleClass > Console;
}

#if defined( MIST_DEBUG )
#define LOG( format, ... ) \
	utils::Console->write( \
		"%.*s%s:%4i\n" format "\n", \
		75 - ( int )sizeof( __FILE__ ), \
		"----------------------------------------" \
		"----------------------------------------", \
		__FILE__, __LINE__, ##__VA_ARGS__ )
#define MARKER() \
	utils::Console->write( \
		"%.*s%s:%4i\n", \
		75 - ( int )sizeof( __FILE__ ), \
		"----------------------------------------" \
		"----------------------------------------", \
		__FILE__, __LINE__ )
#else
#define LOG( format, ... ) \
	utils::Console->write( \
		format "\n", \
		##__VA_ARGS__ )
#define MARKER()
#endif
