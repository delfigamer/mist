#include <exl/node/block.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	Block::Block()
	{
	}

	Block::Block( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::block );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == 0 );
		m_nodes.reserve( s[ 2 ].list.size() );
		for( SExpr const& it: s[ 2 ].list )
		{
			Ref< IStatement > node = constructstatement( it );
			m_nodes.push_back( std::move( node ) );
		}
	}

	Block::~Block()
	{
	}

	Ref< StringBuilder > Block::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			if( m_nodes.size() == 0 )
			{
				m_defstring = StringBuilder::construct( "" );
			}
			else
			{
				for( Ref< IStatement >& node: m_nodes )
				{
					m_defstring = StringBuilder::construct(
						m_defstring, "\n", lineprefix( depth ),
						node->getdefstring( depth ) );
				}
			}
		}
		return m_defstring;
	}
}
