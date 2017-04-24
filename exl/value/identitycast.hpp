#pragma once

#include <exl/value/proxy.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class IdentityCast: public ProxyValue, public virtual ITypeCast
	{
	public:
		IdentityCast( IValue* target );
		~IdentityCast();

		virtual StringBuilder getdefstring( size_t depth ) override;
	};
}
