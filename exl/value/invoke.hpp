#pragma once

#include <exl/value/base.hpp>
#include <exl/types.hpp>
#include <common.hpp>
#include <vector>

namespace exl
{
	class InvokeValue: public Value
	{
	private:
		Ref< IValue > m_target;
		std::vector< Ref< IValue > > m_args;
		Ref< IInvocation > m_invocation;

	public:
		InvokeValue( IValue* target, std::vector< Ref< IValue > > args );
		~InvokeValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< IConstValue > getconstvalue() override;
	};
}
