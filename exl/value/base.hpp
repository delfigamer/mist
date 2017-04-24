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
		virtual uint64_t compileread( ILBody* body ) override;
		virtual void compilewrite( ILBody* body, uint64_t value ) override;
	};
}
