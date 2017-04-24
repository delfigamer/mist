#include <exl/value/literal.hpp>

namespace exl
{
	LiteralValue::LiteralValue()
	{
	}

	LiteralValue::~LiteralValue()
	{
	}

	Ref< IConstValue > LiteralValue::getconstvalue()
	{
		return this;
	}

	Ref< ITypeInfo > LiteralValue::gettypevalue()
	{
		return nullptr;
	}

	Ref< INumberValue > LiteralValue::getnumbervalue()
	{
		return nullptr;
	}

	Ref< IStringValue > LiteralValue::getstringvalue()
	{
		return nullptr;
	}
}
