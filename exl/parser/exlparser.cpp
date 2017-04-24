#include <exl/parser/exlparser.hpp>
#include <exl/parser/exllexer.hpp>
#include <exl/parser/ast.hpp>
#include <utils/console.hpp>
#include <map>
#include <set>
#include <stack>
#include <cstring>

namespace exl
{
	namespace
	{
		char const* tokenname( ExlTokenType type )
		{
			switch( type )
			{
			case ExlTokenType::identifier:
				return "identifier";
			case ExlTokenType::number:
				return "number";
			case ExlTokenType::string:
				return "string";
			case ExlTokenType::eof:
				return "eof";
			case ExlTokenType::newline:
				return "newline";
			case ExlTokenType::kwas:
				return "as";
			case ExlTokenType::kwclass:
				return "class";
			case ExlTokenType::kwconst:
				return "const";
			case ExlTokenType::kwend:
				return "end";
			case ExlTokenType::kwfunction:
				return "function";
			case ExlTokenType::kwin:
				return "in";
			case ExlTokenType::kwinout:
				return "inout";
			case ExlTokenType::kwlocal:
				return "local";
			case ExlTokenType::kwnil:
				return "nil";
			case ExlTokenType::kwout:
				return "out";
			case ExlTokenType::kwtype:
				return "type";
			case ExlTokenType::kwunit:
				return "unit";
			case ExlTokenType::leftroundbracket:
				return "(";
			case ExlTokenType::rightroundbracket:
				return ")";
			case ExlTokenType::leftsquarebracket:
				return "[";
			case ExlTokenType::rightsquarebracket:
				return "]";
			case ExlTokenType::leftbrace:
				return "{";
			case ExlTokenType::rightbrace:
				return "}";
			case ExlTokenType::semicolon:
				return ";";
			case ExlTokenType::colon:
				return ":";
			case ExlTokenType::bang:
				return "!";
			case ExlTokenType::equals:
				return "=";
			case ExlTokenType::equalsequals:
				return "==";
			case ExlTokenType::plus:
				return "+";
			case ExlTokenType::plusequals:
				return "+=";
			case ExlTokenType::minus:
				return "-";
			case ExlTokenType::minusequals:
				return "-=";
			case ExlTokenType::star:
				return "*";
			case ExlTokenType::starequals:
				return "*=";
			case ExlTokenType::slash:
				return "/";
			case ExlTokenType::slashequals:
				return "/";
			case ExlTokenType::tilde:
				return "~";
			case ExlTokenType::tildeequals:
				return "~=";
			case ExlTokenType::pound:
				return "#";
			case ExlTokenType::comma:
				return ",";
			case ExlTokenType::dot:
				return ".";
			case ExlTokenType::dotdot:
				return "..";
			case ExlTokenType::dotdotdot:
				return "...";
			default:
				return "";
			}
		}

		void dumptokens( ExlTokenStream& ts )
		{
			while( true )
			{
				std::unique_ptr< ExlToken > token = ts.gett();
				if( token->bislinestart )
				{
					LOG( "" );
				}
				switch( token->type )
				{
				case ExlTokenType::number:
					LOG( "[%3i:%3i]-[%3i:%3i]\t%s\t%f",
						token->spos.row, token->spos.col,
						token->epos.row, token->epos.col,
						tokenname( token->type ),
						token->number );
				break;

				case ExlTokenType::identifier:
				case ExlTokenType::string:
					if( token->data )
					{
						LOG( "[%3i:%3i]-[%3i:%3i]\t%s\t\"%.*s\"",
							token->spos.row, token->spos.col,
							token->epos.row, token->epos.col,
							tokenname( token->type ),
							( int )token->data->m_length,
							( char* )token->data->m_data );
					}
					else
					{
						LOG( "[%3i:%3i]-[%3i:%3i]\t%s\t\"\"",
							token->spos.row, token->spos.col,
							token->epos.row, token->epos.col,
							tokenname( token->type ) );
					}
				break;

				default:
					LOG( "[%3i:%3i]-[%3i:%3i]\t%s",
						token->spos.row, token->spos.col,
						token->epos.row, token->epos.col,
						tokenname( token->type ) );
				break;
				}
				if( token->type == ExlTokenType::eof )
				{
					break;
				}
			}
		}
	}

	namespace parser
	{
		using utils::SExpr;

		SExpr expr_unary( ExlTokenStream& ts );

		SExpr expr( ExlTokenStream& ts );

		SExpr stat( ExlTokenStream& ts );

		SExpr block( ExlTokenStream& ts, uint32_t head,
			SExpr( *statparser )( ExlTokenStream& ),
			std::set< ExlTokenType > const& terms );

		std::unique_ptr< ExlToken > acquiretoken(
			ExlTokenStream& ts, ExlTokenType type, char const* errmsg )
		{
			std::unique_ptr< ExlToken > token = ts.gett();
			if( token->type != type )
			{
				ts.error( errmsg, token->spos );
			}
			return token;
		}

		SExpr acquirenode(
			ExlTokenStream& ts, SExpr( *func )( ExlTokenStream& ts ),
			char const* errmsg )
		{
			SExpr node = func( ts );
			if( !node )
			{
				ts.error( errmsg, ts.peekt()->spos );
			}
			return node;
		}

		SExpr createnamese( char const* name )
		{
			return
				SExpr( NodeHead::name, {
					SExpr::atom( DataBuffer::create( strlen( name ), name ) ) } );
		}

		SExpr acquirename(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			uint32_t head = NodeHead::name;
			std::unique_ptr< ExlToken > token = std::move( lead );
			if( token->type == ExlTokenType::dot )
			{
				head = NodeHead::fullname;
				token = ts.gett();
			}
			SExpr ret( head );
			while( true )
			{
				if( token->type != ExlTokenType::identifier )
				{
					ts.error( "identifier expected", token->spos );
				}
				ret.list.push_back( SExpr::atom( std::move( token->data ) ) );
				token = ts.gett();
				if( token->type != ExlTokenType::dot )
				{
					ts.ungett( std::move( token ) );
					break;
				}
				token = ts.gett();
			}
			ret.list.shrink_to_fit();
			return ret;
		}

		SExpr argdef( ExlTokenStream& ts )
		{
			bool lvalue;
			bool rvalue;
			std::unique_ptr< ExlToken > token = ts.gett();
			TextPos spos = token->spos;
			switch( token->type )
			{
			case ExlTokenType::kwin:
				lvalue = false;
				rvalue = true;
				token = ts.gett();
			break;

			case ExlTokenType::kwout:
				lvalue = true;
				rvalue = false;
				token = ts.gett();
			break;

			case ExlTokenType::kwinout:
				lvalue = true;
				rvalue = true;
				token = ts.gett();
			break;

			default:
				lvalue = false;
				rvalue = true;
			break;
			}
			SExpr targetname;
			SExpr value;
			SExpr typev;
			if( token->type == ExlTokenType::identifier )
			{
				targetname = SExpr::atom( std::move( token->data ) );
				token = ts.gett();
			}
			if( token->type == ExlTokenType::equals )
			{
				value = acquirenode( ts, expr, "initializer expected" );
				token = ts.gett();
			}
			if( token->type == ExlTokenType::colon )
			{
				typev = acquirenode( ts, expr, "type expected" );
			}
			else
			{
				ts.error( "type expected", token->spos );
			}
			return
				SExpr( NodeHead::argdef, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( spos.row ),
						SExpr::atom( spos.col ),
						SExpr::atom( typev[ 1 ][ 3 ].number ),
						SExpr::atom( typev[ 1 ][ 4 ].number ),
						SExpr::atom( ts.filename ) } ),
					SExpr::atom( lvalue ),
					SExpr::atom( rvalue ),
					std::move( targetname ),
					std::move( value ),
					std::move( typev ) } );
		}

		SExpr argdeflist( ExlTokenStream& ts )
		{
			std::unique_ptr< ExlToken > token = acquiretoken(
				ts, ExlTokenType::leftsquarebracket, "[ expected" );
			TextPos spos = token->spos;
			token = ts.gett();
			if( token->type == ExlTokenType::rightsquarebracket )
			{
				return
					SExpr( NodeHead::argdeflist, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( spos.row ),
							SExpr::atom( spos.col ),
							SExpr::atom( token->epos.row ),
							SExpr::atom( token->epos.col ),
							SExpr::atom( ts.filename ) } ),
						SExpr( 0 ) } );
			}
			ts.ungett( std::move( token ) );
			SExpr argdefs( 0 );
			while( true )
			{
				SExpr ad = argdef( ts );
				argdefs.list.push_back( std::move( ad ) );
				token = ts.gett();
				if( token->type == ExlTokenType::rightsquarebracket )
				{
					break;
				}
				else if( token->type != ExlTokenType::comma )
				{
					ts.error( "] expected", token->spos );
				}
			}
			argdefs.list.shrink_to_fit();
			return
				SExpr( NodeHead::argdeflist, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( spos.row ),
						SExpr::atom( spos.col ),
						SExpr::atom( token->epos.row ),
						SExpr::atom( token->epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( argdefs ) } );
		}

		SExpr expr_element_name(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			TextPos spos = lead->spos;
			TextPos epos = lead->epos;
			SExpr name = acquirename( ts, std::move( lead ) );
			return
				SExpr( NodeHead::expr_name, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( spos.row ),
						SExpr::atom( spos.col ),
						SExpr::atom( epos.row ),
						SExpr::atom( epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( name ) } );
		}

		std::set< ExlTokenType > termset_end{
			ExlTokenType::kwend };
		std::set< ExlTokenType > termset_elseend{
			ExlTokenType::kwelse,
			ExlTokenType::kwelseif,
			ExlTokenType::kwend };

		SExpr expr_type_function(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& nlead,
			std::unique_ptr< ExlToken >&& lead )
		{
			SExpr argdefs = acquirenode(
				ts, argdeflist, "argument definitions expected" );
			std::unique_ptr< ExlToken > token = ts.gett();
			if( token->type == ExlTokenType::colon )
			{
				SExpr rettype = acquirenode(
					ts, expr, "return type expected" );
				return
					SExpr( NodeHead::expr_functiontype, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( lead->spos.row ),
							SExpr::atom( lead->spos.col ),
							SExpr::atom( rettype[ 1 ][ 3 ].number ),
							SExpr::atom( rettype[ 1 ][ 4 ].number ),
							SExpr::atom( ts.filename ) } ),
						std::move( argdefs ),
						std::move( rettype ) } );
			}
			else
			{
				ts.ungett( std::move( token ) );
				return
					SExpr( NodeHead::expr_functiontype, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( lead->spos.row ),
							SExpr::atom( lead->spos.col ),
							SExpr::atom( argdefs[ 1 ][ 3 ].number ),
							SExpr::atom( argdefs[ 1 ][ 4 ].number ),
							SExpr::atom( ts.filename ) } ),
						std::move( argdefs ),
						SExpr::atom() } );
			}
		}

		SExpr expr_element_number(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			return
				SExpr( NodeHead::expr_number, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( lead->epos.row ),
						SExpr::atom( lead->epos.col ),
						SExpr::atom( ts.filename ) } ),
					SExpr::atom( lead->number ) } );
		}

		SExpr expr_element_string(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			return
				SExpr( NodeHead::expr_string, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( lead->epos.row ),
						SExpr::atom( lead->epos.col ),
						SExpr::atom( ts.filename ) } ),
					SExpr::atom( std::move( lead->data ) ) } );
		}

		SExpr expr_element_nil(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			return
				SExpr( NodeHead::expr_nil, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( lead->epos.row ),
						SExpr::atom( lead->epos.col ),
						SExpr::atom( ts.filename ) } ) } );
		}

		SExpr expr_element_subexpression(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			SExpr value = acquirenode( ts, expr, "expression expected" );
			std::unique_ptr< ExlToken > token = acquiretoken(
				ts, ExlTokenType::rightroundbracket, ") expected" );
			return
				SExpr( NodeHead::expr_subexpression, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( token->epos.row ),
						SExpr::atom( token->epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( value ) } );
		}

		SExpr expr_element_function(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			SExpr argdefs = acquirenode(
				ts, argdeflist, "argument definitions expected" );
			SExpr rettype;
			std::unique_ptr< ExlToken > token = ts.gett();
			if( token->type == ExlTokenType::colon )
			{
				rettype = acquirenode( ts, expr, "return type expected" );
			}
			else
			{
				ts.ungett( std::move( token ) );
			}
			SExpr body = block( ts, NodeHead::block, stat, termset_end );
			token = ts.gett();
			return
				SExpr( NodeHead::expr_function, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( token->epos.row ),
						SExpr::atom( token->epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( argdefs ),
					std::move( rettype ),
					std::move( body ) } );
		}

		SExpr expr_typeof(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			SExpr expr = acquirenode( ts, expr_unary, "expression expected" );
			return
				SExpr( NodeHead::expr_typeof, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( expr[ 1 ][ 3 ].number ),
						SExpr::atom( expr[ 1 ][ 4 ].number ),
						SExpr::atom( ts.filename ) } ),
					std::move( expr ) } );
		}

		SExpr expr_element_type(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			std::unique_ptr< ExlToken > nlead = ts.gett();
			switch( nlead->type )
			{
			case ExlTokenType::kwfunction:
				return expr_type_function(
					ts, std::move( nlead ), std::move( lead ) );

			default:
				ts.ungett( std::move( nlead ) );
				return expr_typeof( ts, std::move( lead ) );
			}
		}

		SExpr expr_element( ExlTokenStream& ts )
		{
			std::unique_ptr< ExlToken > lead = ts.gett();
			switch( lead->type )
			{
			case ExlTokenType::number:
				return expr_element_number( ts, std::move( lead ) );

			case ExlTokenType::string:
				return expr_element_string( ts, std::move( lead ) );

			case ExlTokenType::kwnil:
				return expr_element_nil( ts, std::move( lead ) );

			case ExlTokenType::identifier:
			case ExlTokenType::dot:
				return expr_element_name( ts, std::move( lead ) );

			case ExlTokenType::leftroundbracket:
				return expr_element_subexpression( ts, std::move( lead ) );

			case ExlTokenType::kwfunction:
				return expr_element_function( ts, std::move( lead ) );

			case ExlTokenType::kwtype:
				return expr_element_type( ts, std::move( lead ) );

			default:
				ts.ungett( std::move( lead ) );
				return SExpr::atom();
			}
		}

		std::map< ExlTokenType, SExpr > unaryname = {
			{ ExlTokenType::plus, createnamese( "identity" ) },
			{ ExlTokenType::minus, createnamese( "negate" ) },
		};

		SExpr expr_unary( ExlTokenStream& ts )
		{
			std::stack< std::unique_ptr< ExlToken > > prefixes;
			while( true )
			{
				std::unique_ptr< ExlToken > sign = ts.gett();
				if( unaryname.find( sign->type ) == unaryname.end() )
				{
					ts.ungett( std::move( sign ) );
					break;
				}
				prefixes.push( std::move( sign ) );
			}
			if( prefixes.empty() )
			{
				return expr_element( ts );
			}
			SExpr ret = acquirenode( ts, expr_element, "expression expected" );
			double eposrow = ret[ 1 ][ 3 ].number;
			double eposcol = ret[ 1 ][ 4 ].number;
			while( !prefixes.empty() )
			{
				std::unique_ptr< ExlToken > sign = std::move( prefixes.top() );
				ret =
					SExpr( NodeHead::expr_invoke, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( sign->spos.row ),
							SExpr::atom( sign->spos.col ),
							SExpr::atom( eposrow ),
							SExpr::atom( eposcol ),
							SExpr::atom( ts.filename ) } ),
						SExpr( NodeHead::expr_name, {
							SExpr( NodeHead::textrange, {
								SExpr::atom( sign->spos.row ),
								SExpr::atom( sign->spos.col ),
								SExpr::atom( sign->epos.row ),
								SExpr::atom( sign->epos.col ),
								SExpr::atom( ts.filename ) } ),
							unaryname[ sign->type ] } ),
						SExpr( 0, {
							std::move( ret ) } ) } );
				prefixes.pop();
			}
			return ret;
		}

		SExpr expr_suffix_invoke(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead, SExpr&& base )
		{
			std::unique_ptr< ExlToken > token = ts.gett();
			if( token->type == ExlTokenType::rightsquarebracket )
			{
				return
					SExpr( NodeHead::expr_invoke, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( base[ 1 ][ 1 ].number ),
							SExpr::atom( base[ 1 ][ 2 ].number ),
							SExpr::atom( token->epos.row ),
							SExpr::atom( token->epos.col ),
							SExpr::atom( ts.filename ) } ),
						std::move( base ),
						SExpr( 0 ) } );
			}
			ts.ungett( std::move( token ) );
			SExpr args( 0 );
			while( true )
			{
				SExpr arg = acquirenode( ts, expr, "argument expected" );
				args.list.push_back( std::move( arg ) );
				token = ts.gett();
				if( token->type == ExlTokenType::rightsquarebracket )
				{
					break;
				}
				else if( token->type != ExlTokenType::comma )
				{
					ts.error( "] expected", token->spos );
				}
			}
			args.list.shrink_to_fit();
			return
				SExpr( NodeHead::expr_invoke, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( base[ 1 ][ 1 ].number ),
						SExpr::atom( base[ 1 ][ 2 ].number ),
						SExpr::atom( token->epos.row ),
						SExpr::atom( token->epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( base ),
					std::move( args ) } );
		}

		SExpr expr_suffix_cast(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead, SExpr&& base )
		{
			std::unique_ptr< ExlToken > token = ts.gett();
			bool lvalue;
			bool rvalue;
			if( token->type == ExlTokenType::kwin )
			{
				lvalue = false;
				rvalue = true;
			}
			else if( token->type == ExlTokenType::kwout )
			{
				lvalue = true;
				rvalue = false;
			}
			else if( token->type == ExlTokenType::kwout )
			{
				lvalue = true;
				rvalue = true;
			}
			else
			{
				lvalue = false;
				rvalue = true;
				ts.ungett( std::move( token ) );
			}
			SExpr typev = acquirenode( ts, expr_unary, "type expected" );
			return
				SExpr( NodeHead::expr_cast, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( base[ 1 ][ 1 ].number ),
						SExpr::atom( base[ 1 ][ 2 ].number ),
						SExpr::atom( typev[ 1 ][ 3 ].number ),
						SExpr::atom( typev[ 1 ][ 4 ].number ),
						SExpr::atom( ts.filename ) } ),
					std::move( base ),
					SExpr::atom( lvalue ),
					SExpr::atom( rvalue ),
					std::move( typev ) } );
		}

		SExpr expr_suffix( ExlTokenStream& ts )
		{
			SExpr base = expr_unary( ts );
			if( !base )
			{
				return base;
			}
			while( true )
			{
				std::unique_ptr< ExlToken > lead = ts.gett();
				switch( lead->type )
				{
				case ExlTokenType::leftsquarebracket:
					base = expr_suffix_invoke(
						ts, std::move( lead ), std::move( base ) );
				break;

				case ExlTokenType::kwas:
					base = expr_suffix_cast(
						ts, std::move( lead ), std::move( base ) );
				break;

				default:
					ts.ungett( std::move( lead ) );
					return base;
				}
			}
		}

		SExpr expr_binary(
			ExlTokenStream& ts,
			SExpr( *first )( ExlTokenStream& ),
			SExpr( *next )( ExlTokenStream& ),
			std::map< ExlTokenType, SExpr > const& signs )
		{
			SExpr ret = first( ts );
			if( !ret )
			{
				return ret;
			}
			double sposrow = ret[ 1 ][ 1 ].number;
			double sposcol = ret[ 1 ][ 2 ].number;
			while( true )
			{
				std::unique_ptr< ExlToken > sign = ts.gett();
				auto sit = signs.find( sign->type );
				if( sit == signs.end() )
				{
					ts.ungett( std::move( sign ) );
					break;
				}
				SExpr rs = acquirenode( ts, next, "expression expected" );
				ret =
					SExpr( NodeHead::expr_invoke, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( sposrow ),
							SExpr::atom( sposcol ),
							SExpr::atom( rs[ 1 ][ 3 ].number ),
							SExpr::atom( rs[ 1 ][ 4 ].number ),
							SExpr::atom( ts.filename ) } ),
						SExpr( NodeHead::expr_name, {
							SExpr( NodeHead::textrange, {
								SExpr::atom( sign->spos.row ),
								SExpr::atom( sign->spos.col ),
								SExpr::atom( sign->epos.row ),
								SExpr::atom( sign->epos.col ),
								SExpr::atom( ts.filename ) } ),
							sit->second } ),
						SExpr( 0, {
							std::move( ret ),
							std::move( rs ) } ) } );
			}
			return ret;
		}

		SExpr expr_rbinary(
			ExlTokenStream& ts,
			SExpr( *first )( ExlTokenStream& ),
			SExpr( *next )( ExlTokenStream& ),
			std::map< ExlTokenType, SExpr > const& signs )
		{
			SExpr ret = first( ts );
			if( !ret )
			{
				return ret;
			}
			std::stack< SExpr > elist;
			elist.push( std::move( ret ) );
			std::stack< SExpr > slist;
			while( true )
			{
				std::unique_ptr< ExlToken > sign = ts.gett();
				auto sit = signs.find( sign->type );
				if( sit == signs.end() )
				{
					ts.ungett( std::move( sign ) );
					break;
				}
				SExpr rs = acquirenode( ts, next, "expression expected" );
				elist.push( std::move( rs ) );
				slist.push(
					SExpr( NodeHead::expr_name, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( sign->spos.row ),
							SExpr::atom( sign->spos.col ),
							SExpr::atom( sign->epos.row ),
							SExpr::atom( sign->epos.col ),
							SExpr::atom( ts.filename ) } ),
						sit->second } ) );
			}
			ret = std::move( elist.top() );
			elist.pop();
			double eposrow = ret[ 1 ][ 3 ].number;
			double eposcol = ret[ 1 ][ 4 ].number;
			while( !elist.empty() )
			{
				SExpr ls = std::move( elist.top() );
				SExpr signop = std::move( slist.top() );
				ret =
					SExpr( NodeHead::expr_invoke, {
						SExpr( NodeHead::textrange, {
							SExpr::atom( ls[ 1 ][ 1 ].number ),
							SExpr::atom( ls[ 1 ][ 2 ].number ),
							SExpr::atom( eposrow ),
							SExpr::atom( eposcol ),
							SExpr::atom( ts.filename ) } ),
						std::move( signop ),
						SExpr( 0, {
							std::move( ls ),
							std::move( ret ) } ) } );
				elist.pop();
				slist.pop();
			}
			return ret;
		}

		SExpr expr_join( ExlTokenStream& ts )
		{
			static std::map< ExlTokenType, SExpr > signmap{
					{ ExlTokenType::dotdot, createnamese( "join" ) },
				};
			return expr_binary( ts, expr_suffix, expr_suffix, signmap );
		}

		SExpr expr_product( ExlTokenStream& ts )
		{
			static std::map< ExlTokenType, SExpr > signmap{
					{ ExlTokenType::star, createnamese( "multiply" ) },
					{ ExlTokenType::slash, createnamese( "divide" ) },
				};
			return expr_binary( ts, expr_join, expr_join, signmap );
		}

		SExpr expr_sum( ExlTokenStream& ts )
		{
			static std::map< ExlTokenType, SExpr > signmap{
					{ ExlTokenType::plus, createnamese( "add" ) },
					{ ExlTokenType::minus, createnamese( "subtract" ) },
				};
			return expr_binary( ts, expr_product, expr_product, signmap );
		}

		SExpr expr_assignment( ExlTokenStream& ts )
		{
			static std::map< ExlTokenType, SExpr > signmap{
					{ ExlTokenType::equals, createnamese( "assign" ) },
					{ ExlTokenType::plusequals, createnamese( "addto" ) },
					{ ExlTokenType::minusequals, createnamese( "subtractfrom" ) },
				};
			return expr_rbinary( ts, expr_sum, expr_sum, signmap );
		}

		SExpr expr( ExlTokenStream& ts )
		{
			return expr_assignment( ts );
		}

		SExpr stat_const(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			std::unique_ptr< ExlToken > targetname = acquiretoken(
				ts, ExlTokenType::identifier, "identifier expected" );
			acquiretoken(
				ts, ExlTokenType::equals, "= expected" );
			SExpr value = acquirenode( ts, expr, "expression expected" );
			return
				SExpr( NodeHead::stat_const, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( value[ 1 ][ 3 ].number ),
						SExpr::atom( value[ 1 ][ 4 ].number ),
						SExpr::atom( ts.filename ) } ),
					SExpr::atom( std::move( targetname->data ) ),
					std::move( value ) } );
		}

		SExpr stat_function(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			std::unique_ptr< ExlToken > token = ts.gett();
			SExpr targetname = acquirename( ts, std::move( token ) );
			SExpr argdefs = acquirenode(
				ts, argdeflist, "argument definitions expected" );
			SExpr rettype;
			token = ts.gett();
			if( token->type == ExlTokenType::colon )
			{
				rettype = acquirenode( ts, expr, "return type expected" );
			}
			else
			{
				ts.ungett( std::move( token ) );
			}
			SExpr body = block( ts, NodeHead::block, stat, termset_end );
			token = ts.gett();
			return
				SExpr( NodeHead::stat_function, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( token->epos.row ),
						SExpr::atom( token->epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( targetname ),
					std::move( argdefs ),
					std::move( rettype ),
					std::move( body ) } );
		}

		SExpr stat_local(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			std::unique_ptr< ExlToken > token = acquiretoken(
				ts, ExlTokenType::identifier, "identifier expected" );
			SExpr targetname = SExpr::atom( std::move( token->data ) );
			double eposrow = token->epos.row;
			double eposcol = token->epos.col;
			SExpr value;
			SExpr typev;
			token = ts.gett();
			if( token->type == ExlTokenType::equals )
			{
				value = acquirenode( ts, expr, "initializer expected" );
				eposrow = value[ 1 ][ 3 ].number;
				eposcol = value[ 1 ][ 4 ].number;
				token = ts.gett();
			}
			if( token->type == ExlTokenType::colon )
			{
				typev = acquirenode( ts, expr, "type expected" );
				eposrow = typev[ 1 ][ 3 ].number;
				eposcol = typev[ 1 ][ 4 ].number;
			}
			else
			{
				ts.ungett( std::move( token ) );
			}
			return
				SExpr( NodeHead::stat_local, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( eposrow ),
						SExpr::atom( eposcol ),
						SExpr::atom( ts.filename ) } ),
					std::move( targetname ),
					std::move( value ),
					std::move( typev ) } );
		}

		SExpr stat_conditional(
			ExlTokenStream& ts, std::unique_ptr< ExlToken >&& lead )
		{
			std::unique_ptr< ExlToken > token;
			SExpr branches( 0 );
			while( true )
			{
				SExpr condition = acquirenode( ts, expr, "condition expected" );
				token = acquiretoken(
					ts, ExlTokenType::kwthen, "'then' expected" );
				SExpr body = block( ts, NodeHead::block, stat, termset_elseend );
				branches.append(
					SExpr( NodeHead::branch, {
						std::move( condition ),
						std::move( body ) } ) );
				token = ts.gett();
				if( token->type == ExlTokenType::kwelse )
				{
					SExpr body = block( ts, NodeHead::block, stat, termset_end );
					branches.append(
						SExpr( NodeHead::branch, {
							SExpr::atom(),
							std::move( body ) } ) );
					token = ts.gett();
					break;
				}
				else if( token->type == ExlTokenType::kwend )
				{
					break;
				}
			}
			return
				SExpr( NodeHead::stat_conditional, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( lead->spos.row ),
						SExpr::atom( lead->spos.col ),
						SExpr::atom( token->epos.row ),
						SExpr::atom( token->epos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( branches ) } );
		}

		SExpr stat_expr( ExlTokenStream& ts )
		{
			SExpr value = expr( ts );
			if( !value )
			{
				return value;
			}
			return
				SExpr( NodeHead::stat_expression, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( value[ 1 ][ 1 ].number ),
						SExpr::atom( value[ 1 ][ 2 ].number ),
						SExpr::atom( value[ 1 ][ 3 ].number ),
						SExpr::atom( value[ 1 ][ 4 ].number ),
						SExpr::atom( ts.filename ) } ),
					std::move( value ) } );
		}

		SExpr stat( ExlTokenStream& ts )
		{
			std::unique_ptr< ExlToken > lead = ts.gett();
			switch( lead->type )
			{
			case ExlTokenType::kwconst:
				return stat_const( ts, std::move( lead ) );

			case ExlTokenType::kwfunction:
				return stat_function( ts, std::move( lead ) );

			case ExlTokenType::kwlocal:
				return stat_local( ts, std::move( lead ) );

			case ExlTokenType::kwif:
				return stat_conditional( ts, std::move( lead ) );

			default:
				ts.ungett( std::move( lead ) );
				return stat_expr( ts );
			}
		}

		SExpr block( ExlTokenStream& ts, uint32_t head,
			SExpr( *statparser )( ExlTokenStream& ),
			std::set< ExlTokenType > const& terms )
		{
			TextPos spos = ts.peekt()->spos;
			SExpr statements( 0 );
			while( true )
			{
				std::unique_ptr< ExlToken > const& token = ts.peekt();
				if( token->type == ExlTokenType::eof )
				{
					ts.error( "block end expected", token->spos );
				}
				else if( terms.find( token->type ) != terms.end() )
				{
					break;
				}
				SExpr stat = acquirenode( ts, statparser, "statement expected" );
				statements.list.push_back( stat );
			}
			statements.list.shrink_to_fit();
			return
				SExpr( head, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( spos.row ),
						SExpr::atom( spos.col ),
						SExpr::atom( ts.peekt()->spos.row ),
						SExpr::atom( ts.peekt()->spos.col ),
						SExpr::atom( ts.filename ) } ),
					std::move( statements ) } );
		}

		SExpr file( ExlTokenStream& ts )
		{
			SExpr body = parser::block(
				ts, NodeHead::block, parser::stat, termset_end );
			return
				SExpr( NodeHead::file_unit, {
					SExpr( NodeHead::textrange, {
						SExpr::atom( body[ 1 ][ 1 ].number ),
						SExpr::atom( body[ 1 ][ 2 ].number ),
						SExpr::atom( body[ 1 ][ 3 ].number ),
						SExpr::atom( body[ 1 ][ 4 ].number ),
						SExpr::atom( ts.filename ) } ),
					SExpr::atom(),
					std::move( body ) } );
		}
	}

	utils::SExpr* ExlParser::parse(
		char const* const* source, size_t const* sourcelengths,
		size_t partcount, char const* filename, int tabsize )
	{
		ExlTokenStream ts( source, sourcelengths, partcount, filename, tabsize );
		return new utils::SExpr( parser::file( ts ) );
	}
}
