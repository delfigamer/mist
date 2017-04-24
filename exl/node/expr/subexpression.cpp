#include <exl/node/expr/subexpression.hpp>
#include <exl/value/proxy.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/error.hpp>
#include <exl/format.hpp>

namespace exl
{
	SubExpr::SubExpr( utils::SExpr const& s )
		: Expression( s )
	{
		ASSERT( s.head == NodeHead::expr_subexpression );
		ASSERT( s.list.size() == 2 );
		m_expr = constructexpression( s[ 2 ] );
	}

	SubExpr::~SubExpr()
	{
	}

	StringBuilder SubExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "( "_db << m_expr->getdefstring( depth ) << " )"_db;
		return defstring;
	}

	void SubExpr::build( IContext* context )
	{
		m_expr->build( context );
		Ref< IValue > target = m_expr->getvalue();
		ASSERT( target );
		m_value = Ref< ProxyValue >::create( target );
	}
}
