#include <exl/symbol/const.hpp>
#include <exl/format.hpp>

namespace exl
{
	ConstSymbol::ConstSymbol( DefPos defpos, IConstValue* constvalue )
		: Symbol( defpos )
		, m_constvalue( constvalue )
	{
		ASSERT( constvalue );
		m_fulltype = FullType{ m_constvalue->getfulltype().type, true, false };
	}

	ConstSymbol::~ConstSymbol()
	{
	}

	StringBuilder ConstSymbol::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring
			<< "const = "_db << m_constvalue->getdefstring( depth );
		if( m_fulltype.type )
		{
			defstring << ": "_db << m_fulltype.type->getdefstring( depth );
		}
		defstring << " at "_db << defpostostr( m_defpos );
		return defstring;
	}

	Ref< IConstValue > ConstSymbol::getconstvalue()
	{
		return m_constvalue;
	}

	void ConstSymbol::compilewrite(
		ILBody* body, uint64_t base, uint64_t value )
	{
		ASSERT( false );
	}
}
