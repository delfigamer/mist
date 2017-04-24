#include <exl/value/identitycast.hpp>

namespace exl
{
	IdentityCast::IdentityCast( IValue* target )
		: ProxyValue( target )
	{
	}

	IdentityCast::~IdentityCast()
	{
	}

	StringBuilder IdentityCast::getdefstring( size_t depth )
	{
		return m_target->getdefstring( depth );
	}
}
