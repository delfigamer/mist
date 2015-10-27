#ifndef WINDOW_HOSTMETHODLIST_HPP__
#define WINDOW_HOSTMETHODLIST_HPP__ 1

#include "methodlist.hpp"
#ifdef _WIN32
#include <graphics-d3d/methodlist.hpp>
#elif defined(__ANDROID__)
//#include <graphics-gles/methodlist.hpp>
#endif
#include <rsbin/methodlist.hpp>
#include <utils/methodlist.hpp>

namespace window {
	struct HostMethodList {
		utils::MethodList const* utils;
		rsbin::MethodList const* rsbin;
//		graphics::MethodList const* graphics;
		void const* graphics;
		MethodList const* window;
	};
	
	HostMethodList const* gethostmethodlist();
}

#endif
