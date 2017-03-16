#include <exl/luainterface.hpp>
#include <exl/func.hpp>
#include <cstring>

namespace exl
{
	NodeContainer::NodeContainer( Ref< IBlock > const& node )
		: m_node( node )
	{
	}

	NodeContainer::NodeContainer( Ref< IBlock >&& node )
		: m_node( std::move( node ) )
	{
	}

	NodeContainer::~NodeContainer()
	{
	}

	NodeContainer* NodeContainer::create( utils::SExpr const* s )
	{
		return new NodeContainer( constructblock( *s ) );
	}

	DataBuffer* NodeContainer::getdefstring()
	{
		if( !m_defstring )
		{
			Ref< StringBuilder > sb = m_node->getdefstring( 0 );
			size_t len = sb->getlength();
			m_defstring = DataBuffer::create( len, len, nullptr );
			sb->write( m_defstring->m_data, len );
		}
		return m_defstring;
	}
}
