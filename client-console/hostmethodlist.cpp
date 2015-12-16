#include "hostmethodlist.hpp"

#include "methodlist.hpp"
#include <rsbin/methodlist.hpp>
#include <utils/methodlist.hpp>

namespace window
{
	HostMethodList const* gethostmethodlist()
	{
		static HostMethodList const List = {
			utils::getmethodlist(),
			rsbin::getmethodlist(),
			0,
			getmethodlist(),
		};
		return &List;
	}
}
