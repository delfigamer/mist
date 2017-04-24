#pragma once

#include <exl/value/base.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class ReferenceValue: public Value
	{
	private:
		Ref< ISymbol > m_target;

	public:
		ReferenceValue( ISymbol* target );
		~ReferenceValue();

		virtual StringBuilder getdefstring( size_t depth ) override;
		virtual Ref< IConstValue > getconstvalue() override;
		virtual uint64_t compileread( ILBody* body ) override;
		virtual void compilewrite( ILBody* body, uint64_t value ) override;
	};
}
