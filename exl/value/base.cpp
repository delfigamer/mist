#include <exl/value/base.hpp>
#include <exl/il/types.hpp>

namespace exl
{
	Value::Value()
		: m_fulltype{ nullptr, true, false }
	{
	}

	Value::~Value()
	{
	}

	FullType Value::getfulltype()
	{
		return m_fulltype;
	}

	Ref< IConstValue > Value::getconstvalue()
	{
		return nullptr;
	}

	void Value::compileread( ILBody* body, ILValue& value )
	{
		value.setnil();
	}

	void Value::compilewrite( ILBody* body, ILValue const& value )
	{
		ASSERT( false );
	}
}
