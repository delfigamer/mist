#pragma once

#include <exl/object.hpp>
#include <exl/il/types.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class Symbol: public Object, public virtual ISymbol
	{
	protected:
		DefPos m_defpos;
		FullType m_fulltype;
		ILValue m_value;

	public:
		Symbol( DefPos defpos );
		~Symbol();

		virtual DefPos getdefpos() override;
		virtual FullType getfulltype() override;
		virtual void setvalue( ILValue const& value ) override;
		virtual void compileread(
			ILBody* body, ILValue const& base, ILValue& value ) override;
		virtual void compilewrite(
			ILBody* body, ILValue const& base, ILValue const& value ) override;
	};
}
