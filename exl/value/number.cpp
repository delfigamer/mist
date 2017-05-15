#include <exl/value/number.hpp>
#include <exl/system/typeinfo/number.hpp>
#include <exl/il/types.hpp>
#include <exl/func.hpp>
#include <exl/format.hpp>

namespace exl
{
	NumberValue::NumberValue( double content )
		: m_content( content )
	{
		m_fulltype.type = getnumberti();
	}

	NumberValue::~NumberValue()
	{
	}

	StringBuilder NumberValue::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << numtostr( m_content );
		return defstring;
	}

	void NumberValue::compileread( ILBody* body, ILValue& value )
	{
		value.setnumber( m_content );
	}

	Ref< INumberValue > NumberValue::getnumbervalue()
	{
		return this;
	}

	double NumberValue::getnumbercontent()
	{
		return m_content;
	}
}
