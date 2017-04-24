#include <exl/value/typedef.hpp>
#include <exl/system/typeinfo/typedef.hpp>

namespace exl
{
	TypedefValue::TypedefValue(
		ITypeInfo* typevalue, StringBuilder const& defstring )
		: m_defstring( defstring )
		, m_typevalue( typevalue )
	{
		m_fulltype.type = gettypedefti();
	}

	TypedefValue::~TypedefValue()
	{
	}

	StringBuilder TypedefValue::getdefstring( size_t depth )
	{
		return m_defstring;
	}

	Ref< ITypeInfo > TypedefValue::gettypevalue()
	{
		return m_typevalue;
	}
}
