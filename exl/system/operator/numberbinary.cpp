#include <exl/system/operator/numberbinary.hpp>
#include <exl/value/number.hpp>
#include <exl/system/typeinfo/number.hpp>

namespace exl
{
	InvNumberBinary::InvNumberBinary( ITypeCast* left, ITypeCast* right )
		: InvBinary( left, right )
	{
		m_fulltype.type = getnumberti();
	}

	InvNumberBinary::~InvNumberBinary()
	{
	}

	void InvNumberBinary::constructconstvalue()
	{
		Ref< IConstValue > leftconst = m_left->getconstvalue();
		if( !leftconst )
		{
			return;
		}
		Ref< INumberValue > leftnumber = leftconst->getnumbervalue();
		if( !leftnumber )
		{
			return;
		}
		Ref< IConstValue > rightconst = m_right->getconstvalue();
		if( !rightconst )
		{
			return;
		}
		Ref< INumberValue > rightnumber = rightconst->getnumbervalue();
		if( !rightnumber )
		{
			return;
		}
		m_constvalue = Ref< NumberValue >::create( staticvalue(
			leftnumber->getnumbercontent(), rightnumber->getnumbercontent() ) );
	}

	OpNumberBinaryTI::OpNumberBinaryTI()
	{
		m_argtypeinfo = getnumberti();
	}

	OpNumberBinaryTI::~OpNumberBinaryTI()
	{
	}
}
