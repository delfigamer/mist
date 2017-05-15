#pragma once

#include <exl/node/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class ConditionalStat: public Node, public virtual IStatement
	{
	private:
		struct branch_t
		{
			Ref< IExpression > condexpr;
			Ref< IBlock > block;
			Ref< IContext > context;
			Ref< IValue > condvalue;
		};

	private:
		std::vector< branch_t > m_branches;

	public:
		ConditionalStat( utils::SExpr const& s );
		~ConditionalStat();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
		virtual void compile( ILBody* body ) override;
	};
}
