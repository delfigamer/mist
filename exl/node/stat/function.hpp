#pragma once

#include <exl/symbol/const.hpp>
#include <exl/node/base.hpp>
#include <exl/node/argdeflist.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class FunctionStat: public Node, public virtual IStatement
	{
	private:
		name_t m_name;
		bool m_fullname;
		Ref< ArgDefList > m_argdeflist;
		Ref< IExpression > m_rettype;
		Ref< IBlock > m_body;
		Ref< ConstSymbol > m_symbol;

	public:
		FunctionStat( utils::SExpr const& s );
		~FunctionStat();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual void compile( ILBody* body ) override;
	};
}
