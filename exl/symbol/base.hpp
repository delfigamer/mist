#pragma once

#include <exl/object.hpp>
#include <exl/types.hpp>
#include <common.hpp>

namespace exl
{
	class Symbol: public Object, public virtual ISymbol
	{
	protected:
		DefPos m_defpos;
		FullType m_fulltype;
		uint64_t m_register;

	public:
		Symbol( DefPos defpos );
		~Symbol();

		virtual DefPos getdefpos() override;
		virtual FullType getfulltype() override;
		virtual void setregister( uint64_t reg ) override;
		virtual uint64_t compileread(
			ILBody* body, uint64_t base ) override;
		virtual void compilewrite(
			ILBody* body, uint64_t base, uint64_t value ) override;
	};
}
