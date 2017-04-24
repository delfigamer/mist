#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class InvokeExpr: public Expression
	{
	private:
		bool m_simplename;
		identifier_t m_name;
		Ref< IExpression > m_target;
		std::vector< Ref< IExpression > > m_args;

	public:
		InvokeExpr( utils::SExpr const& s );
		~InvokeExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
