#include <exl/node/stat/expression.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	ExpressionStat::ExpressionStat()
	{
	}

	ExpressionStat::ExpressionStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_expression );
		ASSERT( s.list.size() == 2 );
		m_value = constructexpression( s[ 2 ] );
	}

	ExpressionStat::~ExpressionStat()
	{
	}

	Ref< StringBuilder > ExpressionStat::getdefstring( size_t depth )
	{
		if( !m_defstring )
		{
			m_defstring = m_value->getdefstring( depth );
		}
		return m_defstring;
	}
}
