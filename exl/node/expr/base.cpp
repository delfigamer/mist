#include <exl/node/expr/base.hpp>

namespace exl
{
	using utils::SExpr;

	Expression::Expression( utils::SExpr const& s )
		: Node( s )
	{
	}

	Expression::~Expression()
	{
	}

	Ref< IValue > Expression::getvalue()
	{
		return m_value;
	}
}
