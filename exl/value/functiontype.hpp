#pragma once

#include <exl/value/literal.hpp>
#include <exl/node/argdeflist.hpp>
#include <common.hpp>

namespace exl
{
	class FunctionTypeValue: public LiteralValue
	{
	private:
		Ref< ITypeInfo > m_typevalue;

	public:
		FunctionTypeValue(
			std::vector< ArgDef > const& argdeflist, ITypeInfo* rettypeinfo );
		~FunctionTypeValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< ITypeInfo > gettypevalue() override;
	};
}
