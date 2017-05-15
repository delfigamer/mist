#include <exl/system/operator/binary.hpp>
#include <exl/func.hpp>

namespace exl
{
	InvBinary::InvBinary( ITypeCast* left, ITypeCast* right )
		: m_left( left )
		, m_right( right )
	{
	}

	InvBinary::~InvBinary()
	{
	}

	Ref< IConstValue > InvBinary::getconstvalue()
	{
		return m_constvalue;
	}

	void InvBinary::compileread( ILBody* body, ILValue& value )
	{
		if( m_constvalue )
		{
			m_constvalue->compileread( body, value );
		}
		else
		{
			ILValue aval;
			m_left->compileread( body, aval );
			ILValue bval;
			m_right->compileread( body, bval );
			ILValue rval;
			body->newtemp( rval );
			ILToken* op = body->appendtoken();
			op->type = m_operatortype;
			op->inputs.resize( 2 );
			op->inputs[ 0 ] = aval;
			op->inputs[ 1 ] = bval;
			op->outputs.resize( 1 );
			op->outputs[ 0 ] = rval;
			value = rval;
		}
	}

	OpBinaryTI::OpBinaryTI()
	{
	}

	OpBinaryTI::~OpBinaryTI()
	{
	}

	Ref< IInvocation > OpBinaryTI::invoke(
		IValue* target,
		std::vector< Ref< IValue > > const& args )
	{
		if( args.size() != 2 )
		{
			return nullptr;
		}
		Ref< IValue > left = args[ 0 ];
		FullType leftft = left->getfulltype();
		if( !leftft.readable )
		{
			return nullptr;
		}
		Ref< IValue > right = args[ 1 ];
		FullType rightft = right->getfulltype();
		if( !rightft.readable )
		{
			return nullptr;
		}
		Ref< ITypeCast > leftcast = m_argtypeinfo->cast( left );
		if( !leftcast )
		{
			return nullptr;
		}
		Ref< ITypeCast > rightcast = m_argtypeinfo->cast( right );
		if( !rightcast )
		{
			return nullptr;
		}
		Ref< InvBinary > inv = createinvocation( leftcast, rightcast );
		inv->constructconstvalue();
		return inv;
	}
}
