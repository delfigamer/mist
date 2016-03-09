#ifndef UTILS_CONSOLE_HPP__
#define UTILS_CONSOLE_HPP__ 1

#include <utils/singleton.hpp>
#include <common.hpp>
#include <cstdarg>
#include <mutex>
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

		void writeconsole( char const* str, int length );
		void writefile( char const* str, int length );
		void linestart();

	public:
		ConsoleClass();
		~ConsoleClass();
		ConsoleClass( ConsoleClass const& ) = delete;
		ConsoleClass& operator=( ConsoleClass const& ) = delete;

		void writeraw( char const* str );
		ATTRIBUTE(( __format__( gnu_printf, 2, 0 ) ))
			void vwrite( char const* format, va_list args );
		ATTRIBUTE(( __format__( gnu_printf, 2, 3 ) ))
			void write( char const* format, ... );
		ATTRIBUTE(( __format__( gnu_printf, 2, 0 ) ))
			void vwriteln( char const* format, va_list args );
		ATTRIBUTE(( __format__( gnu_printf, 2, 3 ) ))
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
	utils::Console->writeln( \
		"[%73s:%4i]\t" format, \
		__FILE__, __LINE__, ##__VA_ARGS__ )

#endif
