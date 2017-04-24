#pragma once

#include <exl/symbol/base.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class ConstSymbol: public Symbol
	{
	protected:
		Ref< IConstValue > m_constvalue;

	public:
		ConstSymbol( DefPos defpos, IConstValue* constvalue );
		~ConstSymbol();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< IConstValue > getconstvalue() override;
		virtual void compilewrite(
			ILBody* body, uint64_t base, uint64_t value ) override;
	};
}
