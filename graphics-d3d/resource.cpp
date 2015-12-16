#include "resource.hpp"

namespace graphics
{
	Resource::resourcestack_t Resource::s_dead;
	Resource::mutex_t Resource::s_mutex;

	Resource::Resource()
		: m_lastframe( 0 )
#ifndef GRAPHICS_NOLOG
		, m_console( utils::Console )
#endif
	{
	}

	Resource::~Resource()
	{
	}

	void Resource::destroy()
	{
		lock_t _lock( s_mutex );
		s_dead.push( this );
	}

	void Resource::advance( IDirect3DDevice9* device, int framecount )
	{
		if( m_lastframe < framecount )
		{
			m_lastframe = framecount;
			doadvance( device, framecount );
		}
	}

	void Resource::cleanup()
	{
		lock_t _lock( s_mutex );
		while( !s_dead.empty() )
		{
			delete s_dead.top();
			s_dead.pop();
		}
	}
}
