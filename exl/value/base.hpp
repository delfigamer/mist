#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class Value: public Object, public virtual IValue
	{
	protected:
		FullType m_fulltype;

	public:
		Value();
		~Value();

		virtual FullType getfulltype() override;
		virtual Ref< IConstValue > getconstvalue() override;
		virtual void compileread( ILBody* body, ILValue& value ) override;
		virtual void compilewrite( ILBody* body, ILValue const& value ) override;
	};
}
