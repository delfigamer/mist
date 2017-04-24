#pragma once

#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/ref.hpp>
#include <common.hpp>

namespace exl
{
	Ref< IFileNode > constructfilenode( utils::SExpr const& s );
	Ref< IStatement > constructstatement( utils::SExpr const& s );
	Ref< IExpression > constructexpression( utils::SExpr const& s );
	identifier_t constructidentifier( utils::SExpr const& s );
}
