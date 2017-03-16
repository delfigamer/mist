#include <exl/node/base.hpp>
#include <exl/parser/ast.hpp>

namespace exl
{
	using utils::SExpr;

	Node::Node()
	{
		m_sposrow = 0;
		m_sposcol = 0;
		m_eposrow = 0;
		m_eposcol = 0;
	}

	Node::Node( utils::SExpr const& s )
	{
		ASSERT( s.head < 0x40000000 );
		ASSERT( s.list.size() >= 1 );
		ASSERT( s[ 1 ].head == NodeHead::textrange );
		ASSERT( s[ 1 ].list.size() == 5 );
		ASSERT( s[ 1 ][ 1 ].head == SExpr::head_number );
		ASSERT( s[ 1 ][ 2 ].head == SExpr::head_number );
		ASSERT( s[ 1 ][ 3 ].head == SExpr::head_number );
		ASSERT( s[ 1 ][ 4 ].head == SExpr::head_number );
		ASSERT( s[ 1 ][ 5 ].head == SExpr::head_text );
		m_sposrow = s[ 1 ][ 1 ].number;
		m_sposcol = s[ 1 ][ 2 ].number;
		m_eposrow = s[ 1 ][ 3 ].number;
		m_eposcol = s[ 1 ][ 4 ].number;
		m_filename = s[ 1 ][ 5 ].text;
	}

	Node::~Node()
	{
	}

	void Node::gettextrange(
		int* sr, int* sc, int* er, int* ec, Ref< DataBuffer >* filename )
	{
		*sr = m_sposrow;
		*sc = m_sposcol;
		*er = m_eposrow;
		*ec = m_eposcol;
		*filename = m_filename;
	}
}
