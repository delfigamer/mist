#include "hostmethodlist.hpp"

namespace window {
	HostMethodList const* gethostmethodlist() {
		static HostMethodList const List = {
			utils::getmethodlist(),
			rsbin::getmethodlist(),
			0,
			getmethodlist(),
		};
		return &List;
	}
}
