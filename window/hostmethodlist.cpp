#include "hostmethodlist.hpp"
#include "methodlist.hpp"
#if defined( _WIN32 )
#include <graphics-d3d/methodlist.hpp>
#elif defined(__ANDROID__)
//#include <graphics-gles/methodlist.hpp>
#endif
#include <rsbin/methodlist.hpp>
#include <utils/methodlist.hpp>

namespace window
{
	HostMethodList const* gethostmethodlist()
	{
		static HostMethodList const List = {
			utils::getmethodlist(),
			rsbin::getmethodlist(),
#if defined( _WIN32 )
			graphics::getmethodlist(),
#elif defined(__ANDROID__)
			0,
#endif
			getmethodlist(),
		};
		return &List;
	}
}
