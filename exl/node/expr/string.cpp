#include <exl/node/expr/string.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	StringExpr::StringExpr()
	{
	}

	StringExpr::StringExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_string );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == SExpr::head_text );
		m_content = s[ 2 ].text;
	}

	StringExpr::~StringExpr()
	{
	}

	Ref< StringBuilder > StringExpr::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			m_defstring = StringBuilder::construct( "\"",
				bytestostr( m_content ), "\"" );
		}
		return m_defstring;
	}
}
