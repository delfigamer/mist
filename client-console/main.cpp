#include <client-console/window.hpp>
#include <rsbin/fsthread.hpp>
#include <utils/configset.hpp>
#include <utils/console.hpp>
#include <osapi.hpp>
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

String translateline( wchar_t const* wcmdline )
{
	utils::translation_t translation =
	{
		&utils::encoding::utf16,
		&utils::encoding::utf8,
		wcmdline,
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
	return String( db );
}
#endif

#if defined( _WIN32 ) || defined( _WIN64 )
#define PLATFORM "win"
#elif defined(__ANDROID__)
#define PLATFORM "android"
#endif
#define MAINCONF_PATH PATH_START "main.lc"
#define MAINCONF_STR "_PLATFORM = '" PLATFORM "'; " \
	"_CONSOLE = true; _PATH = [[" PATH_START "]]"

int main( int argc, char const** argv )
{
	utils::Console = new utils::ConsoleClass();
	utils::MainConf = new utils::ConfClass( MAINCONF_PATH, MAINCONF_STR );
	rsbin::FsThread = new rsbin::FsThreadClass();
	LOG( "~ Console application start" );
	window::WindowCreationData wcd;
	try
	{
		String cmdline;
#if defined( _WIN32 ) || defined( _WIN64 )
		wchar_t const* wcmdline = trimpath( GetCommandLineW() );
		cmdline = translateline( wcmdline );
#elif defined( __ANDROID__ )
		if( argc >= 2 )
		{
			cmdline = argv[ 1 ];
		}
#endif
		LOG( "Command line: \"%s\"", cmdline.getchars() );
		utils::MainConf->runcmd( cmdline.getchars() );
		window::Window mainwindow( wcd );
		mainwindow.mainloop();
	}
	catch( std::exception const& e )
	{
		utils::Console->writeln( "! Critical error: %s", e.what() );
	}
	delete rsbin::FsThread;
	delete utils::MainConf;
	LOG( "~ Application end" );
	delete utils::Console;
	return 0;
}
