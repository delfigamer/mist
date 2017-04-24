#include <exl/value/base.hpp>

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

	uint64_t Value::compileread( ILBody* body )
	{
		return 0;
	}

	void Value::compilewrite( ILBody* body, uint64_t value )
	{
		ASSERT( false );
	}
}
