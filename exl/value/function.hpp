#pragma once

#include <exl/value/literal.hpp>
#include <exl/node/argdeflist.hpp>
#include <common.hpp>

namespace exl
{
	class FunctionValue: public LiteralValue
	{
	private:
		std::vector< ArgDef > m_argdefs;
		std::vector< Ref< ISymbol > > m_argsymbols;
		Ref< ITypeInfo > m_rettype;
		Ref< IBlock > m_body;
		Ref< IContext > m_subcontext;

	public:
		FunctionValue(
			std::vector< ArgDef > const& argdefs, ITypeInfo* rettype,
			IBlock* body, IContext* context );
		~FunctionValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual uint64_t compileread( ILBody* body ) override;
	};
}
