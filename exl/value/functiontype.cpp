#include <exl/value/functiontype.hpp>
#include <exl/system/typeinfo/typedef.hpp>
#include <exl/typeinfo/function.hpp>

namespace exl
{
	FunctionTypeValue::FunctionTypeValue(
		std::vector< ArgDef > const& argdeflist, ITypeInfo* rettypeinfo )
	{
		m_fulltype.type = gettypedefti();
		m_typevalue = Ref< FunctionTI >::create( argdeflist, rettypeinfo );
	}

	FunctionTypeValue::~FunctionTypeValue()
	{
	}

	StringBuilder FunctionTypeValue::getdefstring( size_t depth )
	{
		return m_typevalue->getdefstring( depth );
	}

	Ref< ITypeInfo > FunctionTypeValue::gettypevalue()
	{
		return m_typevalue;
	}
}
