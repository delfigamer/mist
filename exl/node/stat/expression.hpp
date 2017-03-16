#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

namespace exl
{
	class ExpressionStat: public Node, public virtual IStatement
	{
	private:
		Ref< IExpression > m_value;

	public:
		ExpressionStat();
		ExpressionStat( utils::SExpr const& s );
		~ExpressionStat();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
