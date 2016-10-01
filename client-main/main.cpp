#include <client-main/window.hpp>
#include <rsbin/fsthread.hpp>
#include <utils/configset.hpp>
#include <utils/console.hpp>
#include <osapi.hpp>
#if defined( CON_TARGET )
#elif defined( _WIN32 ) || defined( _WIN64 )
#include <common/databuffer.hpp>
#include <utils/encoding.hpp>
#include <windows.h>
#elif defined(__ANDROID__)
#include <android_native_app_glue.h>
#else
#error "Unsupported target platform"
#endif
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
#define MAINCONF_STR "_PLATFORM = '" PLATFORM "'; _PATH = [[" PATH_START "]]"

#if defined( _WIN32 ) || defined( _WIN64 )
int CALLBACK WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#elif defined(__ANDROID__)
void android_main( android_app* app )
#endif
{
#if defined(__ANDROID__)
	app_dummy();
#endif
	utils::Console = new utils::ConsoleClass();
	utils::MainConf = new utils::ConfClass( MAINCONF_PATH, MAINCONF_STR );
	rsbin::FsThread = new rsbin::FsThreadClass();
	LOG( "~ Application start" );
	window::WindowCreationData wcd;
	try
	{
		String cmdline;
#if defined( _WIN32 ) || defined( _WIN64 )
		wcd.hInstance = hInstance;
		wchar_t const* wcmdline = trimpath( GetCommandLineW() );
		cmdline = translateline( wcmdline );
#elif defined(__ANDROID__)
		wcd.app = app;
#endif
		LOG( "Command line: \"%s\"", cmdline.getchars() );
		utils::MainConf->runcmd( cmdline.getchars() );
		window::Window mainwindow( wcd );
		mainwindow.mainloop();
	}
	catch( const std::exception& e )
	{
		utils::Console->writeln( "! Critical error: %s", e.what() );
	}
	delete rsbin::FsThread;
	delete utils::MainConf;
	LOG( "~ Application end" );
	delete utils::Console;
#if defined( _WIN32 ) || defined( _WIN64 )
	return 0;
#elif defined(__ANDROID__)
#endif
}
