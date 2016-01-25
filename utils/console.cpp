#include "console.hpp"
#include "encoding.hpp"
#include <cstring>
#include <ctime>

namespace utils
{
	void ConsoleClass::writeconsole( char const* str, int length )
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		if( m_outputhandle == 0 )
		{
			return;
		}
		if( m_outputisconsole )
		{
			while( length > 0 )
			{
				wchar_t buffer[ 256 ];
				translation_t trstruct =
				{
					&encoding::utf8, // senc
					&encoding::utf16, // denc
					str, // source
					buffer, // dest
					size_t( length ), // sourcesize
					sizeof( buffer ), // destsize
					0xfffd, // defaultchar
				};
				int trresult = utils::translatestr( &trstruct );
				DWORD wcresult;
				if( !WriteConsoleW(
					m_outputhandle, buffer, trstruct.destresult / 2, &wcresult, 0 ) )
				{
					fprintf( stderr, "%i\n", __LINE__ );
					winerror();
				}
				str += trstruct.sourceresult;
				length -= trstruct.sourceresult;
				switch( trresult )
				{
				case translate_success:
				case translate_source_overrun:
					return;

				case translate_dest_unsupported:
					fprintf( stderr, "%i\n", __LINE__ );
					winerror();
					break;

				case translate_dest_overrun:
					break;
				}
			}
		}
		else
		{
			DWORD result;
			if( !WriteFile( m_outputhandle, str, length, &result, 0 ) )
			{
				fprintf( stderr, "%i\n", __LINE__ );
				winerror();
			}
		}
#else
		printf( "%.*s", length, str );
#endif
	}

	void ConsoleClass::writefile( char const* str, int length )
	{
		fprintf( m_file, "%.*s", length, str );
	}

	ConsoleClass::ConsoleClass()
		: m_newline( true )
	{
#ifdef __ANDROID__
		m_file = fopen( "/storage/sdcard0/Android/data/me.sheimi.sgit/files/repo/mist/output/last.log", "w" );
#else
		m_file = fopen( "last.log", "wb" );
#endif
#if defined( _WIN32 ) || defined( _WIN64 )
#if defined( DISABLE_CONSOLE )
		m_inputhandle = 0;
		m_outputhandle = 0;
#else
		m_inputhandle = GetStdHandle( STD_INPUT_HANDLE );
		if( m_inputhandle == INVALID_HANDLE_VALUE )
		{
			fprintf( stderr, "%i\n", __LINE__ );
			winerror();
		}
		DWORD imode;
		if( m_inputhandle == 0 )
		{
		}
		else if( GetConsoleMode( m_inputhandle, &imode ) )
		{
			m_inputisconsole = true;
		}
		else
		{
			BY_HANDLE_FILE_INFORMATION fi;
			if( GetFileInformationByHandle( m_inputhandle, &fi ) )
			{
				m_inputisconsole = false;
			}
			else
			{
				fprintf( stderr, "%i\n", __LINE__ );
				winerror();
			}
		}
		m_outputhandle = GetStdHandle( STD_OUTPUT_HANDLE );
		if( m_outputhandle == INVALID_HANDLE_VALUE )
		{
			fprintf( stderr, "%i\n", __LINE__ );
			winerror();
		}
		if( m_outputhandle == 0 )
		{
		}
		else if( GetConsoleMode( m_outputhandle, &imode ) )
		{
			m_outputisconsole = true;
		}
		else
		{
			BY_HANDLE_FILE_INFORMATION fi;
			if( GetFileInformationByHandle( m_outputhandle, &fi ) )
			{
				m_outputisconsole = false;
			}
			else
			{
				fprintf( stderr, "%i\n", __LINE__ );
				winerror();
			}
		}
#endif
#endif
	}

	ConsoleClass::~ConsoleClass()
	{
		writeraw( "\nLog finished\n" );
		fclose( m_file );
	}

	void ConsoleClass::linestart()
	{
		if( m_newline )
		{
			m_newline = false;
			char buffer[ 32 ];
			int length = snprintf( buffer, sizeof( buffer ), "[%10u] ", uint32_t( clock() * 1000 / CLOCKS_PER_SEC ) );
			writefile( buffer, length - 1 );
		}
	}

	void ConsoleClass::writeraw( char const* str )
	{
		lock_t lock( m_mutex );
		while( str[ 0 ] )
		{
			linestart();
			char const* npos = strchr( str, '\n' );
			if( npos )
			{
				writefile( str, npos - str + 1 );
				writeconsole( str, npos - str + 1 );
				m_newline = true;
				str = npos + 1;
			}
			else
			{
				int len = strlen( str );
				writefile( str, len );
				writeconsole( str, len );
				break;
			}
		}
	}

	void ConsoleClass::vwrite( char const* format, va_list args )
	{
		lock_t lock( m_mutex );
		va_list args2;
		va_copy( args2, args );
		int length = vsnprintf( 0, 0, format, args2 );
		if( length < 1023 )
		{
			char buffer[ 1024 ];
			vsnprintf( buffer, sizeof( buffer ), format, args );
			writeraw( buffer );
		}
		else
		{
			char* buffer = new char[ length + 1 ];
			vsnprintf( buffer, length + 1, format, args );
			writeraw( buffer );
			delete[] buffer;
		}
		va_end( args2 );
	}

	void ConsoleClass::write( char const* format, ... )
	{
		va_list args;
		va_start( args, format );
		vwrite( format, args );
		va_end( args );
	}

	void ConsoleClass::vwriteln( char const* format, va_list args )
	{
		lock_t lock( m_mutex );
		vwrite( format, args );
		writeraw( "\n" );
		flush();
	}

	void ConsoleClass::writeln( char const* format, ... )
	{
		va_list args;
		va_start( args, format );
		vwriteln( format, args );
		va_end( args );
	}

	void ConsoleClass::writeln()
	{
		lock_t lock( m_mutex );
		writeraw( "\n" );
		flush();
	}

	void ConsoleClass::flush()
	{
		lock_t lock( m_mutex );
		fflush( m_file );
#if defined( _WIN32 ) || defined( _WIN64 )
		if( m_outputhandle != 0 && !m_outputisconsole )
		{
			if( !FlushFileBuffers( m_outputhandle ) )
			{
				fprintf( stderr, "%i\n", __LINE__ );
				winerror();
			}
		}
#else
		fflush( stdout );
#endif
	}

	void ConsoleClass::lock()
	{
		m_mutex.lock();
	}

	void ConsoleClass::unlock()
	{
		m_mutex.unlock();
	}

	void ConsoleClass::getchar( char* str )
	{
		size_t pointlength;
#if defined( _WIN32 ) || defined( _WIN64 )
		if( m_inputhandle == 0 )
		{
			str[ 0 ] = 0;
			return;
		}
		static uint32_t newline_consumed = 0;
		uint32_t charcode;
		if( m_inputisconsole )
		{
			wchar_t buffer[ 2 ] = { 0 };
			int length = 0;
			while( true )
			{
				DWORD result;
				if( !ReadConsoleW( m_inputhandle, buffer + length, 1, &result, 0 ) )
				{
					fprintf( stderr, "%i\n", __LINE__ );
					winerror();
				}
				length += 1;
				size_t pointlength;
				encoding::utf16.decode(
					buffer, &charcode, length * 2, &pointlength );
				if( pointlength != 0 )
				{
					break;
				}
			}
		}
		else
		{
			str[ 0 ] = 0;
			int length = 0;
			while( true )
			{
				DWORD result;
				if( !ReadFile( m_inputhandle, str + length, 1, &result, 0 ) )
				{
					fprintf( stderr, "%i\n", __LINE__ );
					winerror();
				}
				length += 1;
				size_t pointlength;
				encoding::utf8.decode(
					str, &charcode, length, &pointlength );
				if( pointlength != 0 )
				{
					break;
				}
			}
		}
		if( charcode == '\n' || charcode == '\r' )
		{
			if( newline_consumed && newline_consumed != charcode )
			{
				newline_consumed = 0;
				getchar( str );
				return;
			}
			else
			{
				newline_consumed = charcode;
				charcode = '\n';
			}
		}
		else
		{
			newline_consumed = 0;
		}
		if( !encoding::utf8.encode( str, charcode, 4, &pointlength ) )
		{
			str[ 0 ] = 0;
		}
		else
		{
			str[ pointlength ] = 0;
		}
#else
		int length = 0;
		while( true )
		{
			scanf( "%c", str + length );
			length += 1;
			if( !encoding::utf8.decode( str, 0, length, &pointlength ) )
			{
				if( pointlength != 0 )
				{
					str[ 0 ] = 0;
					break;
				}
			}
			else
			{
				str[ pointlength ] = 0;
				break;
			}
		}
#endif
		linestart();
		writefile( str, pointlength );
		if( str[ 0 ] == '\n' )
		{
			m_newline = true;
		}
	}

	utils::Singleton< ConsoleClass > Console;
}
