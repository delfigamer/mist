#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class Expression: public Node, public virtual IExpression
	{
	public:
		Expression();
		Expression( utils::SExpr const& s );
		~Expression();
	};
}
