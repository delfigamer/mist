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

	void Symbol::setregister( uint64_t reg )
	{
		m_register = reg;
	}

	uint64_t Symbol::compileread(
		ILBody* body, uint64_t base )
	{
		uint64_t ret = reservereg( body );
		appendtoken( body, makeregassignment( ret, m_register ) );
		return ret;
	}

	void Symbol::compilewrite(
		ILBody* body, uint64_t base, uint64_t value )
	{
		appendtoken( body, makeregassignment( m_register, value ) );
		releasereg( body, value );
	}
}
