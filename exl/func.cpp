#include <exl/func.hpp>
#include <exl/error.hpp>

namespace exl
{
	identifier_t anonymousident( TextRange range )
	{
		if( range.filename )
		{
			char str[ 2048 ];
			int len = snprintf( str, sizeof( str ), "<%.*s:%i:%i~%i:%i>",
				( int )range.filename->m_length,
				( char const* )range.filename->m_data,
				range.spos.row, range.spos.col,
				range.epos.row, range.epos.col );
			return identifier_t( str, str + len );
		}
		else
		{
			char str[ 256 ];
			int len = snprintf( str, sizeof( str ), "<:%i:%i~%i:%i>",
				range.spos.row, range.spos.col,
				range.epos.row, range.epos.col );
			return identifier_t( str, str + len );
		}
	}

	identifier_t anonymousident( DefPos defpos )
	{
		if( defpos.filename )
		{
			char str[ 2048 ];
			int len = snprintf( str, sizeof( str ), "<%.*s:%i:%i>",
				( int )defpos.filename->m_length,
				( char const* )defpos.filename->m_data,
				defpos.pos.row, defpos.pos.col );
			return identifier_t( str, str + len );
		}
		else
		{
			char str[ 256 ];
			int len = snprintf( str, sizeof( str ), "<:%i:%i>",
				defpos.pos.row, defpos.pos.col );
			return identifier_t( str, str + len );
		}
	}

	name_t appendident( name_t const& prefix, identifier_t const& ident )
	{
		name_t ret;
		ret.reserve( prefix.size() + 1 );
		ret.insert( ret.end(), prefix.begin(), prefix.end() );
		ret.push_back( ident );
		return ret;
	}

	Ref< ITypeInfo > gettypevalue( IExpression* expr )
	{
		Ref< IValue > value = expr->getvalue();
		ASSERT( value );
		Ref< IConstValue > constv = value->getconstvalue();
		if( constv )
		{
			Ref< ITypeInfo > ti = constv->gettypevalue();
			if( ti )
			{
				return ti;
			}
		}
		typeerror( expr->gettextrange() );
	}

	void setsymbol(
		IContext* context, identifier_t const& name, ISymbol* symbol )
	{
		Ref< ISymbol > oldsymbol = context->trysetsymbol( name, symbol );
		if( oldsymbol )
		{
			duplicatesymbolerror( symbol, name, oldsymbol );
		}
	}
}
