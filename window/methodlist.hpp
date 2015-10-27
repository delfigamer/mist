#ifndef WINDOW_METHODLIST_HPP__
#define WINDOW_METHODLIST_HPP__ 1

#include "window.hpp"

namespace window {
	struct MethodList {
		typedef graphics::Shape Shape;

		bool( *window_setshape )( Window* window, Shape* shape );
		WindowInfo const*( *window_getinfo )( Window* window );
		bool( *window_finish )( Window* window );
	};

	MethodList const* getmethodlist();
}

#endif
