#ifndef GRAPHICS_RESOURCE_HPP__
#define GRAPHICS_RESOURCE_HPP__ 1

#ifdef GRAPHICS_NOLOG
#define LOG( ... )
#else
#include <utils/console.hpp>
#endif
#include <utils/refobject.hpp>
#include <utils/counterlock.hpp>
#include <mutex>
#include <stack>
#include <d3d9.h>

namespace graphics
{
	class Resource: public utils::RefObject
	{
	private:
		typedef std::stack< Resource* > resourcestack_t;
		typedef utils::CounterLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		static resourcestack_t s_dead;
		static mutex_t s_mutex;

	protected:
		int m_lastframe;
#ifndef GRAPHICS_NOLOG
		utils::SingletonRef< utils::ConsoleClass > m_console;
#endif

	protected:
		virtual void doadvance( IDirect3DDevice9* device, int framecount ) = 0;

	public:
		Resource();
		virtual ~Resource() override;
		virtual void destroy() override;
		Resource( Resource const& other ) = delete;
		Resource( Resource&& other ) = delete;
		Resource& operator=( Resource const& other ) = delete;
		Resource& operator=( Resource&& other ) = delete;

		void advance( IDirect3DDevice9* device, int framecount );

		static void cleanup();
	};
}

#endif
