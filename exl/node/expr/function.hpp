#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/node/argdeflist.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class FunctionExpr: public Expression
	{
	private:
		Ref< ArgDefList > m_argdeflist;
		Ref< IExpression > m_rettype;
		Ref< IBlock > m_body;

	public:
		FunctionExpr( utils::SExpr const& s );
		~FunctionExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
