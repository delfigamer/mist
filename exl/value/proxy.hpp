#pragma once

#include <exl/value/base.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class ProxyValue: public Value
	{
	protected:
		Ref< IValue > m_target;

	public:
		ProxyValue( IValue* target );
		~ProxyValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< IConstValue > getconstvalue() override;
		virtual void compileread( ILBody* body, ILValue& value ) override;
		virtual void compilewrite( ILBody* body, ILValue const& value ) override;
	};
}
