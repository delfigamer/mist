#pragma once

#include <exl/value/literal.hpp>
#include <common.hpp>

namespace exl
{
	class TypedefValue: public LiteralValue
	{
	protected:
		StringBuilder m_defstring;
		Ref< ITypeInfo > m_typevalue;

	public:
		TypedefValue(
			ITypeInfo* typevalue = nullptr,
			StringBuilder const& defstring = StringBuilder() );
		~TypedefValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< ITypeInfo > gettypevalue() override;
	};
}
