#include <exl/node/block.hpp>
#include <exl/il/types.hpp>
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

	void Block::compile( ILBody* body )
	{
		ILConst* fnconst = body->constants[ 0 ].get();
		for( Ref< IStatement >& node: m_nodes )
		{
			TextRange range = node->gettextrange();
			ILToken* note = body->appendtoken();
			note->type = TokenType::breakpoint;
			note->inputs.resize( 4 );
			note->inputs[ 0 ].setstring( 4, "line" );
			note->inputs[ 1 ].setconstant( fnconst );
			note->inputs[ 2 ].setnumber( range.spos.row );
			note->inputs[ 3 ].setnumber( range.spos.col );
			node->compile( body );
		}
		ILToken* note = body->appendtoken();
		note->type = TokenType::breakpoint;
		note->inputs.resize( 4 );
		note->inputs[ 0 ].setstring( 4, "line" );
		note->inputs[ 1 ].setconstant( fnconst );
		note->inputs[ 2 ].setnumber( m_textrange.epos.row );
		note->inputs[ 3 ].setnumber( m_textrange.epos.col );
	}
}
