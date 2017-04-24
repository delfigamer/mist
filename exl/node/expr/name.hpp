#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	class NameExpr: public Expression
	{
	private:
		name_t m_name;
		bool m_fullname;
		Ref< ISymbol > m_target;

	public:
		NameExpr( utils::SExpr const& s );
		~NameExpr();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void build( IContext* context ) override;
	};
}
