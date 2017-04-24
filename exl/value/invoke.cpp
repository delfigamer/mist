#include <exl/value/invoke.hpp>
#include <exl/format.hpp>
#include <map>
#include <cstring>

namespace exl
{
	InvokeValue::InvokeValue(
		IValue* target, std::vector< Ref< IValue > > args )
		: m_target( target )
		, m_args( std::move( args ) )
	{
		Ref< ITypeInfo > targettype = m_target->gettype();
		ASSERT( targettype );
		m_invocation = targettype->invoke( m_target, m_args );
		if( m_invocation )
		{
			m_fulltype = m_invocation->getfulltype();
		}
		else
		{
			invokeerror( gettextrange(), targettype, m_args );
		}
	}

	InvokeValue::~InvokeValue()
	{
	}

	StringBuilder InvokeExpr::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring
			<< m_target->getdefstring( depth ) << "["_db
			<< sbconcat( m_args, depth, ", "_db ) << "]"_db;
		return defstring;
	}

	Ref< IConstValue > InvokeValue::getconstvalue()
	{
		if( m_invocation )
		{
			return m_invocation->getconstvalue();
		}
		else
		{
			return nullptr;
		}
	}
}
