#pragma once

#include <utils/sexpr.hpp>
#include <common.hpp>

namespace exl
{
	R_CLASS( name = exlparser )
	class ExlParser
	{
	public:
		R_METHOD( gc = methodlist.sexpr_delete ) static utils::SExpr* parse(
			char const* const* source, size_t const* sourcelengths,
			size_t partcount, char const* filename, int tabsize );
	};
}
