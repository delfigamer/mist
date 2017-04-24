#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class Expression: public Node, public virtual IExpression
	{
	protected:
		Ref< IValue > m_value;

	public:
		Expression();
		Expression( utils::SExpr const& s );
		~Expression();

		virtual Ref< IValue > getvalue() override;
	};
}
