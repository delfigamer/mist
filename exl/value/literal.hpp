#pragma once

#include <exl/value/base.hpp>
#include <common.hpp>

namespace exl
{
	class LiteralValue: public Value, public virtual IConstValue
	{
	public:
		LiteralValue();
		~LiteralValue();

		virtual Ref< IConstValue > getconstvalue() override;
		virtual Ref< ITypeInfo > gettypevalue() override;
		virtual Ref< INumberValue > getnumbervalue() override;
		virtual Ref< IStringValue > getstringvalue() override;
	};
}
