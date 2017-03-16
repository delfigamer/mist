#pragma once

#include <exl/node/expr/base.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class StringExpr: public Expression
	{
	private:
		Ref< DataBuffer > m_content;

	public:
		StringExpr();
		StringExpr( utils::SExpr const& s );
		~StringExpr();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
