#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

namespace exl
{
	class ConstStat: public Node, public virtual IStatement
	{
	private:
		identifier_t m_targetname;
		Ref< IExpression > m_value;

	public:
		ConstStat();
		ConstStat( utils::SExpr const& s );
		~ConstStat();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
