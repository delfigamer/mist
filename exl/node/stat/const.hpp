#pragma once

#include <exl/symbol/const.hpp>
#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class ConstStat: public Node, public virtual IStatement
	{
	private:
		identifier_t m_targetname;
		Ref< IExpression > m_valueexpr;
		Ref< IContext > m_context;
		Ref< ConstSymbol > m_symbol;
		Ref< IValue > m_value;

	public:
		ConstStat( utils::SExpr const& s );
		~ConstStat();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual void compile( ILBody* body ) override;
	};
}
