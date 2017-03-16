#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>
#include <cinttypes>

namespace exl
{
	class LocalStat: public Node, public virtual IStatement
	{
	private:
		identifier_t m_targetname;
		Ref< IExpression > m_value;
		Ref< IExpression > m_typev;

	public:
		LocalStat();
		LocalStat( utils::SExpr const& s );
		~LocalStat();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
