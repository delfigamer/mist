#include <exl/system/value/operator.hpp>

namespace exl
{
	OperatorValue::OperatorValue(
		ITypeInfo* type, StringBuilder const& defstring )
		: m_defstring( defstring )
	{
		m_fulltype.type = type;
	}

	OperatorValue::~OperatorValue()
	{
	}

	StringBuilder OperatorValue::getdefstring( size_t depth )
	{
		return m_defstring;
	}
}
