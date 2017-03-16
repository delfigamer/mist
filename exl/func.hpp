#pragma once

#include <exl/types.hpp>
#include <utils/sexpr.hpp>
#include <common/databuffer.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace exl
{
	Ref< DataBuffer > const& lineprefix( size_t depth );
	Ref< DataBuffer > numtostr( double number );
	Ref< DataBuffer > bytestostr( Ref< DataBuffer > const& bytes );
	Ref< DataBuffer > identtostr( identifier_t const& ident );

	identifier_t constructidentifier( utils::SExpr const& s );
	Ref< IBlock > constructblock( utils::SExpr const& s );
	Ref< IStatement > constructstatement( utils::SExpr const& s );
	Ref< IExpression > constructexpression( utils::SExpr const& s );
}
