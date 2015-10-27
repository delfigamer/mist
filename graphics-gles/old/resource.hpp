#ifndef GRAPHICS_RESOURCE_HPP__
#define GRAPHICS_RESOURCE_HPP__ 1

#include <utils/monitor.hpp>
#include <mutex>
#include <stack>

namespace graphics {
	class Resource: public utils::Monitor {
	private:
		typedef std::stack< Resource* > resourcestack_t;
		typedef std::mutex mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;
		
	private:
		static resourcestack_t s_dead;
		static mutex_t s_mutex;
		
	public:
		virtual bool query( int id, void** target ) override;
		Resource();
		virtual ~Resource() override;
		virtual void destroy() override;
		Resource( Resource const& other ) = delete;
		Resource( Resource&& other ) = delete;
		Resource& operator=( Resource const& other ) = delete;
		Resource& operator=( Resource&& other ) = delete;
		
		virtual void advance() = 0;
		
		static void cleanup();
	};
}

DEFINE_REFID( graphics::Resource, utils::Monitor, 0x7cbf0acc );

namespace graphics {
	extern "C" {
	}
}

#endif
