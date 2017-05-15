#include <exl/node/stat/expression.hpp>
#include <exl/il/types.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	ExpressionStat::ExpressionStat( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::stat_expression );
		ASSERT( s.list.size() == 2 );
		m_expr = constructexpression( s[ 2 ] );
	}

	ExpressionStat::~ExpressionStat()
	{
	}

	StringBuilder ExpressionStat::getdefstring( size_t depth )
	{
		return m_expr->getdefstring( depth );
	}

	void ExpressionStat::build( IContext* context )
	{
		m_expr->build( context );
		m_value = m_expr->getvalue();
		ASSERT( m_value );
	}

	void ExpressionStat::compile( ILBody* body )
	{
		ILValue value;
		m_value->compileread( body, value );
	}
}
