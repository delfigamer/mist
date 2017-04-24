#include <exl/value/number.hpp>
#include <exl/system/typeinfo/number.hpp>
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

	uint64_t NumberValue::compileread( ILBody* body )
	{
		uint64_t ret = reservereg( body );
		std::unique_ptr< ILNumberValue > value( new ILNumberValue );
			value->type = ValueType::number;
			value->number = m_content;
		appendtoken( body, makeregassignment( ret, std::move( value ) ) );
		return ret;
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
