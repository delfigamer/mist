#include <client-main/window.hpp>
#include <utils/console.hpp>
#if defined( CON_TARGET )
#elif defined( _WIN32 ) || defined( _WIN64 )
#include <utils/databuffer.hpp>
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
#endif

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
	LOG( "~ Application start" );
	window::WindowCreationData wcd;
	try
	{
#if defined( _WIN32 ) || defined( _WIN64 )
		wcd.hInstance = hInstance;
		wchar_t const* cmdline = trimpath( GetCommandLineW() );
		utils::translation_t translation = {
			&utils::encoding::utf16,
			&utils::encoding::utf8,
			cmdline,
			0,
			0,
			0,
			0xfffd,
		};
		if( utils::translatestr( &translation ) != utils::translate_success )
		{
			throw std::runtime_error( "cannot translate command line" );
		}
		utils::Ref< utils::DataBuffer > db = utils::DataBuffer::create(
			translation.destresult, translation.destresult, 0 );
		translation.dest = db->m_data;
		translation.sourcesize = translation.sourceresult;
		translation.destsize = db->m_capacity;
		if( utils::translatestr( &translation ) != utils::translate_success )
		{
			throw std::runtime_error( "cannot translate command line" );
		}
		wcd.cmdline.m_payload = std::move( db );
#elif defined(__ANDROID__)
		wcd.app = app;
		wcd.cmdline = 0;
#endif
		LOG( "Command line: \"%s\"", wcd.cmdline.getchars() );
		window::Window mainwindow( wcd );
		mainwindow.mainloop();
	}
	catch( const std::exception& e )
	{
		utils::Console->writeln( "! Critical error: %s", e.what() );
	}
	LOG( "~ Application end" );
#if defined( _WIN32 ) || defined( _WIN64 )
	return 0;
#elif defined(__ANDROID__)
#endif
}
