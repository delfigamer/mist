#include <exl/node/expr/base.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>

namespace exl
{
	using utils::SExpr;

	Expression::Expression()
	{
	}

	Expression::Expression( utils::SExpr const& s )
		: Node( s )
	{
	}

	Expression::~Expression()
	{
	}
}
