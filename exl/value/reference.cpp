#include <exl/value/reference.hpp>

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

	uint64_t ReferenceValue::compileread( ILBody* body )
	{
		return m_target->compileread( body, 0 );
	}

	void ReferenceValue::compilewrite( ILBody* body, uint64_t value )
	{
		m_target->compilewrite( body, 0, value );
	}
}
