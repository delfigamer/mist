#pragma once

#include <utils/sexpr.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace exl
{
	namespace NodeHead
	{
		enum
		{
			textpos = 0x100,
			textrange = 0x101,
			name = 0x102,
			fullname = 0x103,
			argdef = 0x104,
			argdeflist = 0x105,
			branch = 0x106,
			block = 0x107,
			expr_nil = 0x200,
			expr_number = 0x201,
			expr_string = 0x202,
			expr_name = 0x203,
			expr_subexpression = 0x204,
			expr_function = 0x205,
			expr_functiontype = 0x206,
			expr_typeof = 0x207,
			expr_cast = 0x208,
			expr_invoke = 0x210,
			stat_const = 0x300,
			stat_function = 0x301,
			stat_local = 0x302,
			stat_expression = 0x310,
			stat_conditional = 0x320,
			file_unit = 0x400,
		};
	}

	extern Ref< utils::SENameMap > ASTNameMap;
	extern Ref< utils::SEContext > ASTContext;

	R_CLASS( name = exlast )
	struct AST
	{
		R_METHOD() static utils::SENameMap* getnamemap()
		{
			::addref( ASTNameMap );
			return ASTNameMap;
		}
		R_METHOD() static utils::SEContext* getcontext()
		{
			::addref( ASTContext );
			return ASTContext;
		}
	};
}
