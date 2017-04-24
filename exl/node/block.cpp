#include <exl/node/block.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	Block::Block()
	{
	}

	Block::Block( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::block );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == 0 );
		m_nodes = apply( s[ 2 ].list, constructstatement );
	}

	Block::~Block()
	{
	}

	StringBuilder Block::getdefstring( size_t depth )
	{
		return sbconcat(
			m_nodes, depth, nullptr,
			[]( IStatement* node, size_t depth )
			{
				return StringBuilder()
					<< "\n"_db << lineprefix( depth )
					<< node->getdefstring( depth );
			} );
	}

	void Block::build( IContext* context )
	{
		for( Ref< IStatement >& node: m_nodes )
		{
			node->build( context );
		}
	}

	void Block::compilereserve( ILBody* body )
	{
		for( Ref< IStatement >& node: m_nodes )
		{
			node->compilereserve( body );
		}
	}

	void Block::compileemit( ILBody* body )
	{
		for( Ref< IStatement >& node: m_nodes )
		{
			TextRange range = node->gettextrange();
			std::unique_ptr< ILBreakpointNote > note( new ILBreakpointNote );
				note->type = TokenType::breakpointnote;
				note->pos = range.spos;
				note->filename = range.filename;
			appendtoken( body, std::move( note ) );
			node->compileemit( body );
		}
		std::unique_ptr< ILBreakpointNote > note( new ILBreakpointNote );
			note->type = TokenType::breakpointnote;
			note->pos = m_textrange.epos;
			note->filename = m_textrange.filename;
		appendtoken( body, std::move( note ) );
	}
}
