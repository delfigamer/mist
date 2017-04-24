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

	uint64_t InvBinary::compileread( ILBody* body )
	{
		if( m_constvalue )
		{
			return m_constvalue->compileread( body );
		}
		else
		{
			uint64_t areg = m_left->compileread( body );
			uint64_t breg = m_right->compileread( body );
			std::unique_ptr< ILOperator > op( new ILOperator );
				op->type = m_operatortype;
				op->inputs.push_back( makeregvalue( areg ) );
				op->inputs.push_back( makeregvalue( breg ) );
				op->outputs.push_back( makeregvalue( areg ) );
			appendtoken( body, std::move( op ) );
			releasereg( body, breg );
			return areg;
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
