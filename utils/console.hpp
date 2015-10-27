#ifndef UTILS_CONSOLE_HPP__
#define UTILS_CONSOLE_HPP__ 1

#include "singleton.hpp"
#if defined( _WIN32 ) || defined( _WIN64 )
#include "winapi.hpp"
#endif
#include <cstdio>
#include <cstdarg>
#include <mutex>

namespace utils
{
	class ConsoleClass
	{
	private:
		typedef std::recursive_mutex mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t m_mutex;
		FILE* m_file;
		bool m_newline;
#if defined( _WIN32 ) || defined( _WIN64 )
		HANDLE m_inputhandle;
		bool m_inputisconsole;
		HANDLE m_outputhandle;
		bool m_outputisconsole;
#endif

		void writeconsole( char const* str, int length );
		void writefile( char const* str, int length );
		void linestart();

	public:
		ConsoleClass();
		~ConsoleClass();
		ConsoleClass( ConsoleClass const& ) = delete;
		ConsoleClass( ConsoleClass&& ) = delete;
		ConsoleClass& operator=( ConsoleClass const& ) = delete;
		ConsoleClass& operator=( ConsoleClass&& ) = delete;

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

	extern utils::Singleton< ConsoleClass > Console;
}

#define LOG( format, ... ) \
	utils::Console()->writeln( \
		"[%48s:%24s@%4i]\t" format, \
		__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ )

#endif
