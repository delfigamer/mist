#pragma once

#include <exl/value/literal.hpp>
#include <common.hpp>

namespace exl
{
	class OperatorValue: public LiteralValue
	{
	private:
		StringBuilder m_defstring;

	public:
		OperatorValue( ITypeInfo* type, StringBuilder const& defstring );
		~OperatorValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
	};
}
