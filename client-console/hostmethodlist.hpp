#ifndef WINDOW_HOSTMETHODLIST_HPP__
#define WINDOW_HOSTMETHODLIST_HPP__ 1

#include "methodlist.hpp"
#include <rsbin/methodlist.hpp>
#include <utils/methodlist.hpp>

namespace window {
	struct HostMethodList {
		utils::MethodList const* utils;
		rsbin::MethodList const* rsbin;
		void const* graphics;
		MethodList const* window;
	};

	HostMethodList const* gethostmethodlist();
}

#endif
