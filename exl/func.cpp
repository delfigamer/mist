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

	void initilbody( ILBody* body )
	{
		ILBlock* exitblock;
		ILBlock* entryblock;
		body->blocks.emplace_back( exitblock = new ILBlock );
		body->blocks.emplace_back( entryblock = new ILBlock );
		body->entryblock = entryblock;
		body->exitblock = exitblock;
		body->currentblock = entryblock;
		body->stackpos = 0;
	}

	uint64_t reservereg( ILBody* body )
	{
		if( body->stackpos == 0xffffffff )
		{
			throw std::runtime_error( "maximum block stack size reached" );
		}
		body->stackpos += 1;
		return ( ( uint64_t )body->depth << 32 ) | body->stackpos;
	}

	void releasereg( ILBody* body, uint64_t reg )
	{
		if( reg != 0 )
		{
			uint32_t pos = reg & 0xffffffff;
#if defined( MIST_DEBUG )
			uint32_t depth = reg >> 32;
			ASSERT( pos == body->stackpos );
			ASSERT( depth == body->depth );
#endif
			body->stackpos = pos - 1;
		}
	}

	void createblock( ILBody* body )
	{
		ILBlock* block;
		body->blocks.emplace_back( block = new ILBlock );
		body->currentblock = block;
	}

	void appendtoken( ILBody* body, std::unique_ptr< ILToken >&& token )
	{
		body->currentblock->tokens.push_back( std::move( token ) );
	}

	std::unique_ptr< ILValue > makeregvalue( uint64_t reg )
	{
		std::unique_ptr< ILRegisterValue > ret( new ILRegisterValue );
			ret->type = ValueType::reg;
			ret->reg = reg;
		return std::move( ret );
	}

	std::unique_ptr< ILToken > makeregassignment(
		uint64_t target, uint64_t source )
	{
		std::unique_ptr< ILOperator > op( new ILOperator );
			op->type = TokenType::assign;
			op->inputs.push_back( makeregvalue( source ) );
			op->outputs.push_back( makeregvalue( target ) );
		return std::move( op );
	}

	std::unique_ptr< ILToken > makeregassignment(
		uint64_t target, std::unique_ptr< ILValue >&& source )
	{
		std::unique_ptr< ILOperator > op( new ILOperator );
			op->type = TokenType::assign;
			op->inputs.push_back( std::move( source ) );
			op->outputs.push_back( makeregvalue( target ) );
		return std::move( op );
	}
}
