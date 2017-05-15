#include <exl/symbol/base.hpp>
#include <exl/func.hpp>

namespace exl
{
	Symbol::Symbol( DefPos defpos )
		: m_defpos( defpos )
	{
	}

	Symbol::~Symbol()
	{
	}

	DefPos Symbol::getdefpos()
	{
		return m_defpos;
	}

	FullType Symbol::getfulltype()
	{
		return m_fulltype;
	}

	void Symbol::setvalue( ILValue const& value )
	{
		m_value = value;
	}

	void Symbol::compileread(
		ILBody* body, ILValue const& base, ILValue& value )
	{
		value = m_value;
	}

	void Symbol::compilewrite(
		ILBody* body, ILValue const& base, ILValue const& value )
	{
		body->appendassignment( m_value, value );
	}
}
