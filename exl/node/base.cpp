#include <exl/node/base.hpp>
#include <exl/parser/ast.hpp>

namespace exl
{
	Node::Node()
	{
	}

	Node::Node( utils::SExpr const& s )
	{
		ASSERT( s.head < 0x40000000 );
		ASSERT( s.list.size() >= 1 );
		ASSERT( s[ 1 ].head == NodeHead::textrange );
		ASSERT( s[ 1 ].list.size() == 5 );
		ASSERT( s[ 1 ][ 1 ].head == utils::SExpr::head_number );
		ASSERT( s[ 1 ][ 2 ].head == utils::SExpr::head_number );
		ASSERT( s[ 1 ][ 3 ].head == utils::SExpr::head_number );
		ASSERT( s[ 1 ][ 4 ].head == utils::SExpr::head_number );
		ASSERT( s[ 1 ][ 5 ].head == utils::SExpr::head_text );
		m_textrange.spos.row = s[ 1 ][ 1 ].number;
		m_textrange.spos.col = s[ 1 ][ 2 ].number;
		m_textrange.epos.row = s[ 1 ][ 3 ].number;
		m_textrange.epos.col = s[ 1 ][ 4 ].number;
		m_textrange.filename = s[ 1 ][ 5 ].text;
	}

	Node::~Node()
	{
	}

	TextRange Node::gettextrange()
	{
		return m_textrange;
	}
}
