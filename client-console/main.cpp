#include <client-console/window.hpp>
#include <rsbin/fsthread.hpp>
#include <utils/console.hpp>
#include <common/databuffer.hpp>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <utils/encoding.hpp>
#include <windows.h>
#elif defined(__ANDROID__)
#else
#error "Unsupported target platform"
#endif
#include <stdexcept>
#include <exception>

#if defined( _WIN32 ) || defined( _WIN64 )
static wchar_t const* trimpath( wchar_t const* argstr )
{
	if( argstr[ 0 ] == '"' )
	{
		int rpos = 1;
		while( argstr[ rpos ] )
		{
			if( argstr[ rpos ] == '\'' && argstr[ rpos + 1 ] )
			{
				rpos += 2;
			}
			else if( argstr[ rpos ] == '"' )
			{
				rpos += 1;
				break;
			}
			else
			{
				rpos += 1;
			}
		}
		return argstr + rpos;
	}
	else
	{
		int rpos = 1;
		while( argstr[ rpos ] )
		{
			if( uint16_t( argstr[ rpos ] ) <= uint16_t( ' ' ) )
			{
				rpos += 1;
				break;
			}
			else
			{
				rpos += 1;
			}
		}
		return argstr + rpos;
	}
}
#endif

int main( int argc, char const** argv )
{
	utils::Console = new utils::ConsoleClass();
	rsbin::FsThread = new rsbin::FsThreadClass();
	LOG( "~ Console application start" );
	window::WindowCreationData wcd;
	try
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		wchar_t const* cmdline = trimpath( GetCommandLineW() );
		utils::translation_t translation =
		{
			&utils::encoding::utf16,
			&utils::encoding::utf8,
			cmdline,
			0,
			0,
			0,
			0xfffd,
		};
		if( utils::translatestr( &translation ) !=
			utils::translateresult::success )
		{
			throw std::runtime_error( "cannot translate command line" );
		}
		Ref< DataBuffer > db = DataBuffer::create(
			translation.destresult, translation.destresult, 0 );
		translation.dest = db->m_data;
		translation.sourcesize = translation.sourceresult;
		translation.destsize = db->m_capacity;
		if( utils::translatestr( &translation ) !=
			utils::translateresult::success )
		{
			throw std::runtime_error( "cannot translate command line" );
		}
		wcd.cmdline.m_payload = std::move( db );
#elif defined( __ANDROID__ )
		if( argc >= 2 )
		{
			wcd.cmdline = argv[ 1 ];
		}
#endif
		LOG( "Command line: \"%s\"", wcd.cmdline.getchars() );
		window::Window mainwindow( wcd );
		mainwindow.mainloop();
	}
	catch( std::exception const& e )
	{
		utils::Console->writeln( "! Critical error: %s", e.what() );
	}
	delete rsbin::FsThread;
	LOG( "~ Application end" );
	delete utils::Console;
	return 0;
}
