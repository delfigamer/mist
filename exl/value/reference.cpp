#include <exl/value/reference.hpp>
#include <exl/il/types.hpp>

namespace exl
{
	ReferenceValue::ReferenceValue( ISymbol* target )
		: m_target( target )
	{
		m_fulltype = target->getfulltype();
	}

	ReferenceValue::~ReferenceValue()
	{
	}

	StringBuilder ReferenceValue::getdefstring( size_t depth )
	{
		return "< reference >"_db;
	}

	Ref< IConstValue > ReferenceValue::getconstvalue()
	{
		return m_target->getconstvalue();
	}

	void ReferenceValue::compileread( ILBody* body, ILValue& value )
	{
		ILValue nilv;
		nilv.setnil();
		m_target->compileread( body, nilv, value );
	}

	void ReferenceValue::compilewrite( ILBody* body, ILValue const& value )
	{
		ILValue nilv;
		nilv.setnil();
		m_target->compilewrite( body, nilv, value );
	}
}
