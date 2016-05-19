#include <renderer-d3d9/resource.hpp>
#include <renderer-d3d9/context.hpp>

namespace graphics
{
	Resource::Resource()
		: m_lastframe( 0 )
	{
	}

	Resource::~Resource()
	{
	}

	void Resource::destroy()
	{
		Context::markdead( this );
	}

	void Resource::advance()
	{
		size_t drawnframe = Context::DrawnFrame.load( std::memory_order_relaxed );
		if( m_lastframe < drawnframe )
		{
			m_lastframe = drawnframe;
			doadvance();
		}
	}
}
