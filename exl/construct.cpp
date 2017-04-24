#include <exl/construct.hpp>
#include <exl/node/expr/function.hpp>
#include <exl/node/expr/functiontype.hpp>
#include <exl/node/expr/invoke.hpp>
#include <exl/node/expr/name.hpp>
#include <exl/node/expr/number.hpp>
#include <exl/node/expr/string.hpp>
#include <exl/node/expr/subexpression.hpp>
#include <exl/node/file/unit.hpp>
#include <exl/node/stat/conditional.hpp>
#include <exl/node/stat/const.hpp>
#include <exl/node/stat/function.hpp>
#include <exl/node/stat/expression.hpp>
#include <exl/node/stat/local.hpp>
#include <exl/parser/ast.hpp>
#include <exl/error.hpp>

namespace exl
{
	Ref< IFileNode > constructfilenode( utils::SExpr const& s )
	{
		switch( s.head )
		{
		case NodeHead::file_unit:
			return Ref< UnitFile >::create( s );
		default:
			sexprerror( "invalid file node", s );
		}
	}

	Ref< IStatement > constructstatement( utils::SExpr const& s )
	{
		switch( s.head )
		{
		case NodeHead::stat_conditional:
			return Ref< ConditionalStat >::create( s );
		case NodeHead::stat_const:
			return Ref< ConstStat >::create( s );
		case NodeHead::stat_function:
			return Ref< FunctionStat >::create( s );
		case NodeHead::stat_expression:
			return Ref< ExpressionStat >::create( s );
		case NodeHead::stat_local:
			return Ref< LocalStat >::create( s );
		default:
			sexprerror( "invalid statement node", s );
		}
	}

	Ref< IExpression > constructexpression( utils::SExpr const& s )
	{
		switch( s.head )
		{
		case NodeHead::expr_function:
			return Ref< FunctionExpr >::create( s );
		case NodeHead::expr_functiontype:
			return Ref< FunctionTypeExpr >::create( s );
		case NodeHead::expr_invoke:
			return Ref< InvokeExpr >::create( s );
		case NodeHead::expr_name:
			return Ref< NameExpr >::create( s );
		case NodeHead::expr_number:
			return Ref< NumberExpr >::create( s );
		case NodeHead::expr_string:
			return Ref< StringExpr >::create( s );
		case NodeHead::expr_subexpression:
			return Ref< SubExpr >::create( s );
		default:
			sexprerror( "invalid expression node", s );
		}
	}

	identifier_t constructidentifier( utils::SExpr const& s )
	{
		if( s.head == utils::SExpr::head_text )
		{
			if( s.text )
			{
				return identifier_t(
					s.text->m_data, s.text->m_data + s.text->m_length );
			}
			else
			{
				return identifier_t();
			}
		}
		else
		{
			sexprerror( "invalid identifier node", s );
		}
	}
}
