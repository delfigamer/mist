#include <exl/value/proxy.hpp>

namespace exl
{
	ProxyValue::ProxyValue( IValue* target )
		: m_target( target )
	{
		m_fulltype = target->getfulltype();
	}

	ProxyValue::~ProxyValue()
	{
	}

	StringBuilder ProxyValue::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "( "_db << m_target->getdefstring( depth ) << " )"_db;
		return defstring;
	}

	Ref< IConstValue > ProxyValue::getconstvalue()
	{
		return m_target->getconstvalue();
	}

	void ProxyValue::compileread( ILBody* body, ILValue& value )
	{
		m_target->compileread( body, value );
	}

	void ProxyValue::compilewrite( ILBody* body, ILValue const& value )
	{
		m_target->compilewrite( body, value );
	}
}
