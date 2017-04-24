#pragma once

#include <exl/node/expr/base.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class NumberExpr: public Expression
	{
	private:
		double m_content;

	public:
		NumberExpr( utils::SExpr const& s );
		~NumberExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
