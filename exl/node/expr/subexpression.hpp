#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class SubExpr: public Expression
	{
	private:
		Ref< IExpression > m_expr;

	public:
		SubExpr( utils::SExpr const& s );
		~SubExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
