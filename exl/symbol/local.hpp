#pragma once

#include <exl/symbol/base.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class LocalSymbol: public Symbol
	{
	public:
		LocalSymbol( DefPos defpos, ITypeInfo* typeinfo );
		~LocalSymbol();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< IConstValue > getconstvalue() override;
	};
}
