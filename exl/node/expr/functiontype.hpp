#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/node/argdeflist.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class FunctionTypeExpr: public Expression
	{
	protected:
		Ref< ArgDefList > m_argdeflist;
		Ref< IExpression > m_rettype;
		Ref< ITypeInfo > m_rettypeinfo;

	public:
		FunctionTypeExpr( utils::SExpr const& s );
		~FunctionTypeExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
