#include "resource.hpp"

namespace graphics {
	Resource::resourcestack_t Resource::s_dead;
	Resource::mutex_t Resource::s_mutex;
	
	bool Resource::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}
	
	Resource::Resource() {
	}
	
	Resource::~Resource() {
	}
	
	void Resource::destroy() {
		lock_t _lock( s_mutex );
		s_dead.push( this );
	}
	
	void Resource::cleanup() {
		lock_t _lock( s_mutex );
		while( !s_dead.empty() ) {
			delete s_dead.top();
			s_dead.pop();
		}
	}
}
