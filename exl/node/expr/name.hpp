#pragma once

#include <exl/node/expr/base.hpp>
#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	class NameExpr: public Expression
	{
	private:
		std::vector< identifier_t > m_parts;
		bool m_fullname;

	public:
		NameExpr();
		NameExpr( utils::SExpr const& s );
		~NameExpr();

		virtual Ref< StringBuilder > getdefstring( size_t depth ) override;
	};
}
