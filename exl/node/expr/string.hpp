#pragma once

#include <exl/node/expr/base.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class StringExpr: public Expression
	{
	private:
		Ref< DataBuffer > m_content;

	public:
		StringExpr( utils::SExpr const& s );
		~StringExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
