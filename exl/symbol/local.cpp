#include <exl/symbol/local.hpp>
#include <exl/format.hpp>

namespace exl
{
	LocalSymbol::LocalSymbol( DefPos defpos, ITypeInfo* typeinfo )
		: Symbol( defpos )
	{
		m_fulltype = FullType{ typeinfo, true, true };
	}

	LocalSymbol::~LocalSymbol()
	{
	}

	StringBuilder LocalSymbol::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "local"_db;
		if( m_fulltype.type )
		{
			defstring << ": "_db << m_fulltype.type->getdefstring( depth );
		}
		defstring << " at "_db << defpostostr( m_defpos );
		return defstring;
	}

	Ref< IConstValue > LocalSymbol::getconstvalue()
	{
		return nullptr;
	}
}
