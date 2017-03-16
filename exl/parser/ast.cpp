#include <exl/parser/ast.hpp>

namespace exl
{
	Ref< utils::SENameMap > ASTNameMap( new utils::SENameMap{
		{ NodeHead::textpos, "textpos" },
		{ NodeHead::textrange, "textrange" },
		{ NodeHead::name, "name" },
		{ NodeHead::fullname, "fullname" },
		{ NodeHead::argdef, "argdef" },
		{ NodeHead::argdeflist, "argdeflist" },
		{ NodeHead::expr_nil, "expr.nil" },
		{ NodeHead::expr_number, "expr.number" },
		{ NodeHead::expr_string, "expr.string" },
		{ NodeHead::expr_name, "expr.name" },
		{ NodeHead::expr_subexpression, "expr.subexpression" },
		{ NodeHead::expr_function, "expr.function" },
		{ NodeHead::expr_functiontype, "expr.functiontype" },
		{ NodeHead::expr_typeof, "expr.typeof" },
		{ NodeHead::expr_cast, "expr.cast" },
		{ NodeHead::expr_invoke, "expr.invoke" },
		{ NodeHead::stat_const, "stat.const" },
		{ NodeHead::stat_function, "stat.function" },
		{ NodeHead::stat_local, "stat.local" },
		{ NodeHead::stat_expression, "stat.expression" },
		{ NodeHead::block, "block" },
	}, 0 );

	Ref< utils::SEContext > ASTContext( new utils::SEContext{
	}, 0 );
}
