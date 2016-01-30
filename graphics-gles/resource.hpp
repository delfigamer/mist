#ifndef GRAPHICS_RESOURCE_HPP__
#define GRAPHICS_RESOURCE_HPP__ 1

#include <utils/refobject.hpp>
#include <utils/flaglock.hpp>
#include <mutex>
#include <stack>
#include <GLES2/gl2.h>

namespace graphics
{
	class Resource: public utils::RefObject
	{
	private:
		typedef std::stack< Resource* > resourcestack_t;
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		static resourcestack_t s_dead;
		static mutex_t s_mutex;
		
	protected:
		int m_lastframe;
		
	protected:
		virtual void doadvance( int framecount ) = 0;
		
	public:
		Resource();
		virtual ~Resource() override;
		virtual void destroy() override;
		Resource( Resource const& ) = delete;
		Resource& operator=( Resource const& ) = delete;
		
		void advance( int framecount );
		
		static void cleanup();
	};
}

#endif
