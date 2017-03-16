#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

namespace exl
{
	class InvokeExpr: public Expression
	{
	private:
		Ref< IExpression > m_target;
		std::vector< Ref< IExpression > > m_args;

	public:
		InvokeExpr() = delete;
		InvokeExpr( utils::SExpr const& s );
		~InvokeExpr();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
