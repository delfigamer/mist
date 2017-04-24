#pragma once

#include <exl/types.hpp>
#include <common/ref.hpp>
#include <common.hpp>
#include <vector>

namespace utils
{
	struct SExpr;
}

namespace exl
{
	ATTRIBUTE(( noreturn ))
	void consterror( TextRange range );
	ATTRIBUTE(( noreturn ))
	void duplicatesymbolerror(
		ISymbol* newsymbol, identifier_t const& name, ISymbol* oldsymbol );
	ATTRIBUTE(( noreturn ))
	void initerror(
		TextRange range, ITypeInfo* targettype );
	ATTRIBUTE(( noreturn ))
	void invalidlocalerror( TextRange range );
	ATTRIBUTE(( noreturn ))
	void invokeerror(
		TextRange range, ITypeInfo* targettype,
		std::vector< Ref< IExpression > > const& args );
	ATTRIBUTE(( noreturn ))
	void nyierror( TextRange range, char const* filename, int line );
	ATTRIBUTE(( noreturn ))
	void readableerror( TextRange range );
	ATTRIBUTE(( noreturn ))
	void sexprerror( char const* message, utils::SExpr const& s );
	ATTRIBUTE(( noreturn ))
	void typeerror( TextRange range );
	ATTRIBUTE(( noreturn ))
	void undefinedsymbolerror( TextRange range, identifier_t const& ident );
}
