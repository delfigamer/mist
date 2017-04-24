#include <exl/node/expr/number.hpp>
#include <exl/value/number.hpp>
#include <exl/parser/ast.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	using utils::SExpr;

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

	StringBuilder NumberExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << numtostr( m_content );
		return defstring;
	}

	void NumberExpr::build( IContext* context )
	{
		m_value = Ref< NumberValue >::create( m_content );
	}
}
