#pragma once

#include <exl/node/expr/base.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class NumberExpr: public Expression
	{
	private:
		double m_content;

	public:
		NumberExpr();
		NumberExpr( utils::SExpr const& s );
		~NumberExpr();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
