#ifndef WINDOW_HOSTMETHODLIST_HPP__
#define WINDOW_HOSTMETHODLIST_HPP__ 1

namespace utils
{
	class MethodList;
}

namespace rsbin
{
	class MethodList;
}

namespace graphics
{
	class MethodList;
}

namespace window
{
	class MethodList;

	struct HostMethodList
	{
		utils::MethodList const* utils;
		rsbin::MethodList const* rsbin;
		graphics::MethodList const* graphics;
		MethodList const* window;
	};

	HostMethodList const* gethostmethodlist();
}

#endif
