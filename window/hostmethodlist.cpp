#include "hostmethodlist.hpp"
#include "methodlist.hpp"
#ifdef _WIN32
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
			graphics::getmethodlist(),
			getmethodlist(),
		};
		return &List;
	}
}
