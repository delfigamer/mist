#include <exl/node/expr/number.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	NumberExpr::NumberExpr()
		: m_content( 0 )
	{
	}

	NumberExpr::NumberExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_number );
		ASSERT( s.list.size() == 2 );
		ASSERT( s[ 2 ].head == SExpr::head_number );
		m_content = s[ 2 ].number;
	}

	NumberExpr::~NumberExpr()
	{
	}

	Ref< StringBuilder > NumberExpr::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			m_defstring = StringBuilder::construct( numtostr( m_content ) );
		}
		return m_defstring;
	}
}
