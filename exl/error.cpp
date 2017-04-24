#include <exl/error.hpp>
#include <exl/parser/ast.hpp>
#include <exl/format.hpp>
#include <utils/sexpr.hpp>
#include <common/strexception.hpp>
#include <common/stringbuilder.hpp>
#include <cstring>

namespace exl
{
	using utils::SExpr;

	namespace
	{
		StringBuilder argfulltypetostr( IExpression* argexpr, size_t depth )
		{
			Ref< IValue > arg = argexpr->getvalue();
			ASSERT( arg );
			StringBuilder str;
			FullType ft = arg->getfulltype();
			if( ft.readable )
			{
				if( ft.writable )
				{
					str << "inout: "_db;
				}
				else
				{
					str << ": "_db;
				}
			}
			else
			{
				if( ft.writable )
				{
					str << "out: "_db;
				}
				else
				{
					str << "void"_db;
					return str;
				}
			}
			ASSERT( ft.type );
			str << ft.type->getdefstring( depth );
			return str;
		}
	}

	void consterror( TextRange range )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " given expression does not have a static value"_db );
	}

	void duplicatesymbolerror(
		ISymbol* newsymbol, identifier_t const& name, ISymbol* oldsymbol )
	{
		throw StrException( StringBuilder()
			<< defpostostr( newsymbol->getdefpos() )
			<< " symbol '"_db << identtostr( name )
			<< "' is already defined at "_db
			<< defpostostr( oldsymbol->getdefpos() ) );
	}

	void initerror(
		TextRange range, ITypeInfo* targettype )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " cannot convert given initializer to "_db
			<< targettype->getdefstring( 0 ) );
	}

	void invalidlocalerror( TextRange range )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " initializer or type definition are required to define a "_db
				"local"_db );
	}

	void invokeerror(
		TextRange range, ITypeInfo* targettype,
		std::vector< Ref< IExpression > > const& args )
	{
		StringBuilder errmsg;
		errmsg
			<< textrangetostr( range )
			<< " a '"_db << targettype->getdefstring( 0 )
			<< "' value cannot be invoked with ["_db
			<< sbconcat( args, 0, ", "_db, argfulltypetostr ) << "]"_db;
		throw StrException( errmsg );
	}

	void nyierror( TextRange range, char const* filename, int line )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " NYI "_db
			<< DataBuffer::create( strlen( filename ), filename )
			<< ":"_db << inttostr( line ) );
	}

	void readableerror( TextRange range )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " given expression does not produce a readable value"_db );
	}

	void sexprerror( char const* message, utils::SExpr const& s )
	{
		if( s.head < 0x40000000 &&
			s.list.size() >= 1 &&
			s[ 1 ].head == NodeHead::textrange &&
			s[ 1 ][ 1 ].head == SExpr::head_number &&
			s[ 1 ][ 2 ].head == SExpr::head_number &&
			s[ 1 ][ 3 ].head == SExpr::head_number &&
			s[ 1 ][ 4 ].head == SExpr::head_number &&
			s[ 1 ][ 5 ].head == SExpr::head_text )
		{
			char const* filename;
			int fnlength;
			if( s[ 1 ][ 5 ].text )
			{
				filename = ( char const* )s[ 1 ][ 5 ].text->m_data;
				fnlength = s[ 1 ][ 5 ].text->m_length;
			}
			else
			{
				filename = "";
				fnlength = 0;
			}
			throw StrException( "%.*s:%i:%i~%i:%i %s",
				fnlength, filename,
				( int )s[ 1 ][ 1 ].number, ( int )s[ 1 ][ 2 ].number,
				( int )s[ 1 ][ 3 ].number, ( int )s[ 1 ][ 4 ].number,
				message );
		}
		else
		{
			throw StrException( message );
		}
	}

	void typeerror( TextRange range )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " given expression does not statically define a type"_db );
	}

	void undefinedsymbolerror( TextRange range, identifier_t const& ident )
	{
		throw StrException( StringBuilder()
			<< textrangetostr( range )
			<< " symbol '"_db << identtostr( ident )
			<< "' is not defined"_db );
	}
}
