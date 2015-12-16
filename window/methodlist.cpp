#include "methodlist.hpp"
#include "window.hpp"

namespace window {
	MethodList const* getmethodlist()
	{
		static MethodList const List = {
			window_window_setshape,
			window_window_getinfo,
			window_window_finish
		};
		return &List;
	}
}
