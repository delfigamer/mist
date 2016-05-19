#pragma once

#include <client-console/windowinfo.hpp>
#include <utils/configset.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <lua/lua.hpp>
#include <ctime>

namespace window
{
	struct WindowCreationData
	{
		utils::String cmdline;
	};

	R_CLASS( name = window )
	class Window
	{
	private:
		utils::ConfigSet m_mainconfig;
		utils::String m_cmdline;
		lua_State* m_lstate;
		WindowInfo m_info;
		void initialize();
		void initlstate();

	public:
		Window() = delete;
		explicit Window( WindowCreationData const& wcd );
		Window( Window const& ) = delete;
		Window( Window&& ) = delete;
		~Window();

		void mainloop();
	};
}
