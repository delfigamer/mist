#include <exl/context.hpp>
#include <exl/format.hpp>

namespace exl
{
	Context::Context()
	{
	}

	Context::Context( name_t const& nameprefix, IContext* parent )
		: m_nameprefix( nameprefix )
		, m_parent( parent )
	{
	}

	Context::Context( name_t&& nameprefix, IContext* parent )
		: m_nameprefix( std::move( nameprefix ) )
		, m_parent( parent )
	{
	}

	Context::~Context()
	{
	}

	StringBuilder Context::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring << "context "_db << nametostr( m_nameprefix );
		if( m_parent )
		{
			defstring << " <= "_db << nametostr( m_parent->getnameprefix() );
		}
		defstring << ":"_db;
		for( auto& it: m_symbols )
		{
			identifier_t const& name = it.first;
			Ref< ISymbol > symbol = it.second;
			defstring << "\n"_db << lineprefix( depth + 1 )
				<< identtostr( name ) << " = "_db
				<< symbol->getdefstring( depth + 1 );
		}
		return defstring;
	}

	name_t const& Context::getnameprefix()
	{
		return m_nameprefix;
	}

	Ref< ISymbol > Context::getsymbol( identifier_t const& name )
	{
		auto it = m_symbols.find( name );
		if( it != m_symbols.end() )
		{
			return it->second;
		}
		else if( m_parent )
		{
			return m_parent->getsymbol( name );
		}
		else
		{
			return nullptr;
		}
	}

	Ref< ISymbol > Context::trysetsymbol(
			identifier_t const& name, ISymbol* symbol )
	{
		Ref< ISymbol >& second = m_symbols[ name ];
		if( second )
		{
			return second;
		}
		else
		{
			second = symbol;
			return nullptr;
		}
	}
}
