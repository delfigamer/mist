#if defined( _WIN32 ) || defined( _WIN64 )
#include <windows.h>
#else
#error "This header is for Windows target only"
#endif

namespace utils
{
	void winerror() __attribute__(( noreturn ));
}
