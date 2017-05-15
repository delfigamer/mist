#pragma once

#include <exl/value/literal.hpp>
#include <common.hpp>

namespace exl
{
	class NumberValue: public LiteralValue, public virtual INumberValue
	{
	private:
		double m_content;

	public:
		NumberValue( double content );
		~NumberValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual void compileread( ILBody* body, ILValue& value ) override;
		virtual Ref< INumberValue > getnumbervalue() override;
		virtual double getnumbercontent() override;
	};
}
