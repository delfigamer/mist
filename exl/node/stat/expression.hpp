#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class ExpressionStat: public Node, public virtual IStatement
	{
	private:
		Ref< IExpression > m_expr;
		Ref< IValue > m_value;

	public:
		ExpressionStat( utils::SExpr const& s );
		~ExpressionStat();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual void compilereserve( ILBody* body ) override;
		virtual void compileemit( ILBody* body ) override;
	};
}
